#pragma once

#include <cstring>
#include <memory>
#include <limits>
#include <omp.h>

#include "tbb/concurrent_map.h"
#include "tbb/scalable_allocator.h"
#include "tbb/parallel_for_each.h"

#include "batchedSparseArray.hpp"
#include "densityManager.hpp"
#include "assert.hpp"
#include "typedefs.hpp"

#include "concurrentBucketMap.hpp"

#define USE_BUCKET_MAP true

namespace DataStructures {
struct BinaryRebalancingManagerEntry {
    SizeType numberOfBlocks;
    std::atomic_bool rebalanced;//is being rebalanced in the next phase.

    BinaryRebalancingManagerEntry() :
            numberOfBlocks(0),
            rebalanced(false) {}

    BinaryRebalancingManagerEntry(const BinaryRebalancingManagerEntry &other) :
            numberOfBlocks(other.numberOfBlocks),
            rebalanced(other.rebalanced.load(std::memory_order_relaxed)) {}

    //These operations are not atomic, but this is fine as they are only used during resizing of the hierarchy,
    //where no concurrent operations can occur.
    BinaryRebalancingManagerEntry &operator=(const BinaryRebalancingManagerEntry &other) {
        numberOfBlocks = other.numberOfBlocks;
        rebalanced.store(other.rebalanced.load(std::memory_order_relaxed), std::memory_order_relaxed);
        return *this;
    }
};

template<SizeType SEGMENT_WIDTH, typename SDS_TYPE, typename DENSITY_MANAGER, typename ALLOCATOR>
class BinaryRebalancingManager {
    static constexpr SizeType AggregateWidth = SEGMENT_WIDTH;//Number of blocks that is aggregated by one node on level 0.
    using SdsType = SDS_TYPE;
    using DensityManager = DENSITY_MANAGER;
#if USE_BUCKET_MAP
    using ConcurrentMap = ConcurrentBucketMap;
#else
    using AllocatorType = tbb::scalable_allocator<std::pair<const IndexType, bool>>;
    using ConcurrentMap = oneapi::tbb::concurrent_map<IndexType, bool, std::less<IndexType>, AllocatorType>;
#endif
    static constexpr SizeType LogInitialArrayCapacity = 14;
    static constexpr SizeType InitialArrayCapacity = 1LL << LogInitialArrayCapacity;//The initial capacity (in blocks).
    static constexpr double CapacityFactorForAllLevels = 2.0;//For all levels, we need 2 times the capacity.
#ifndef NDEBUG
    static constexpr SizeType LevelZeroParallelizationThresholdPerThread = 0;//For debugging, use parallel way even for less work.
    static constexpr SizeType HigherLevelParallelizationThresholdPerThread = 0;//For debugging, use parallel way even for less work.
#else
    static constexpr SizeType LevelZeroParallelizationThresholdPerThread = 10;
    static constexpr SizeType HigherLevelParallelizationThresholdPerThread = 100;
#endif

public:
    SdsType *sds;
    std::vector<BinaryRebalancingManagerEntry> aggregates;
    std::vector<IndexType> levelStarts;
    std::vector<IndexType> levelAggregateWidths;//Number of aggregates (!) of level 0, that each aggregate of the level corresponds to.
    SizeType size;
    SizeType logCapacity;
    SizeType logCapacityWithSuperRoot;
    SizeType capacity;
    DensityManager densityManager;
#if !USE_BUCKET_MAP
    AllocatorType allocator;
#endif
    ConcurrentMap nextLevelRecountingNodes;
    UnsignedIndexType numberOfThreads;

public:
    BinaryRebalancingManager(SdsType *pointer, UnsignedIndexType numThreads) :
            sds(pointer),
            logCapacity(LogInitialArrayCapacity),
            logCapacityWithSuperRoot(LogInitialArrayCapacity + 1),
            capacity(InitialArrayCapacity),
            densityManager(),
#if USE_BUCKET_MAP
            nextLevelRecountingNodes(numThreads),
#else
            allocator(),
            nextLevelRecountingNodes(allocator),
#endif
            numberOfThreads(numThreads) {
        aggregates.resize(getSizeForCapacity(InitialArrayCapacity));
        recomputeLevelInfo();
        size = lowestLevelNodeIndex(sds->size) + 1;
        AssertMsg(size <= capacity, "Overflow.");
    }

    /**
     * Returns true, iff the array was reallocated (and needs to be recomputed).
     */
    void resize(SizeType newSdsSize) {
        //std::cout << "Resizing" << std::endl;
        const SizeType newSize = lowestLevelNodeIndex(newSdsSize) + 1;
        const bool sizeChanged = newSize != size;
        size = newSize;
        if (sizeChanged && newSize > capacity) {
            while (capacity <= newSize) {
                ++logCapacity;
                ++logCapacityWithSuperRoot;
                capacity *= 2;//Binary tree gets one level more.
            }
            //Allocate twice the space to have enough space for all levels, setting all entries to zero.
            aggregates.assign(getSizeForCapacity(capacity), BinaryRebalancingManagerEntry{});
            recomputeLevelInfo();
        }
    }

    /**
     * Recomputes the entire auxiliary data after adjusting to a possibly changed sds array size.
     */
    void recompute(SizeType sdsSize) {
        //Recompute the data for level 0:
#pragma omp parallel for schedule(static) num_threads(numberOfThreads)
        for (IndexType i = 0; i < numberOfAggregatesInLevel(0); ++i) {
            SizeType aggregateNumberOfBlocks = 0;
            for (IndexType current = getBlockIndexLevelZero(i); current < std::min(sdsSize, getBlockIndexLevelZero(i + 1)); ++current) {
                aggregateNumberOfBlocks += sds->empty(current) ? 0 : 1;
                aggregateNumberOfBlocks += sds->numberOfBufferBlocks(current);
            }
            AssertMsg(i < 2 * capacity, "Invalid index");
            aggregates[aggregateIndex(i, 0)].numberOfBlocks = aggregateNumberOfBlocks;
        }
        //For the other levels:
        for (IndexType level = 1; level <= logCapacity; ++level) {
#pragma omp parallel for schedule(static) num_threads(numberOfThreads)
            for (IndexType i = 0; i < numberOfAggregatesInLevel(level); ++i) {
                SizeType aggregateNumberOfBlocks = 0;
                AssertMsg(aggregateIndex(leftChild(i), level - 1) < 2 * capacity, "Invalid index");
                AssertMsg(aggregateIndex(rightChild(i), level - 1) < 2 * capacity, "Invalid index");
                aggregateNumberOfBlocks += aggregates[aggregateIndex(leftChild(i), level - 1)].numberOfBlocks;
                aggregateNumberOfBlocks += aggregates[aggregateIndex(rightChild(i), level - 1)].numberOfBlocks;
                AssertMsg(aggregateIndex(i, level) < 2 * capacity, "Invalid index");
                aggregates[aggregateIndex(i, level)].numberOfBlocks = aggregateNumberOfBlocks;
            }
        }
    }

    void update() {
        //Recount all the blocks on level 0, storing their parents for working on the next levels.
        updateLevelZero();

        //std::cout << "Done with level 0" << std::endl;
        //Then, pass all the updated values up to the higher levels.
        for (IndexType level = 1; level <= logCapacity; ++level) {
#if USE_BUCKET_MAP
            std::swap(sds->recountingNodesMap, nextLevelRecountingNodes);
#else
            sds->recountingNodesVector.clear();
            sds->copyRecountingNodesToVector(nextLevelRecountingNodes);
#endif
            nextLevelRecountingNodes.clear();
            updateHigherLevel(level);
            //if (nextLevelRecountingNodes.size() == 0) break;
            //std::cout << "Done with level " << level << std::endl;
        }

        //Check if we need to rebalance globally (super root)
        const SizeType totalNumberOfBlocks = aggregates[aggregateIndex(0, logCapacity)].numberOfBlocks;
        if (totalNumberOfBlocks > AggregateWidth * levelAggregateWidths[logCapacity]) {
            //Global rebalance on the root node
            //std::cout << "Need to rebalance on the super root." << std::endl;
            bool notRebalanced = false;
            const bool update = aggregates[aggregateIndex(0, logCapacity + 1)].rebalanced.compare_exchange_strong(notRebalanced, true, std::memory_order_relaxed, std::memory_order_relaxed);
            AssertMsg(update, "Super root already rebalanced");
            if (update) sds->rebalancingNodes.emplace_back(logCapacity + 1, 0);
        }
    }

    void assertAllRebalancingFlagsCleared() {
        for (IndexType level = 0; level <= logCapacityWithSuperRoot; ++level) {
            for (IndexType i = 0; i < numberOfAggregatesInLevel(level); ++i) {
                AssertMsg(aggregateIndex(i, level) < 2 * capacity, "Invalid index");
                AssertMsg(aggregates[aggregateIndex(i, level)].rebalanced.load(std::memory_order_relaxed) == false, "Rebalanced flag not reset!");
            }
        }
    }

    /**
     * Resets the rebalancing thread of the given rebalancing node to -1.
     */
    void clearRebalancedFlag(const RebalancingNode &node) {
        AssertMsg(aggregateIndex(node.index, node.level) < 2 * capacity, "Invalid index");
        //std::cout << "XXXXXXXXXXX                                   Clearing rebalancing thread for node " << node << std::endl;
        aggregates[aggregateIndex(node.index, node.level)].rebalanced.store(false, std::memory_order_relaxed);
    }

    /**
     * Checks if there is any rebalancing node strictly above the given node that
     * will be rebalanced.
     * Returns true, iff the node is dominated and does not need rebalancing itself.
     * If the node is dominated, resets its rebalancingThread.
     */
    bool rebalancingNodeDominated(const RebalancingNode &node) {
        //std::cout << "   Checking dominance for node " << node << std::endl;
        AssertMsg(aggregates[aggregateIndex(node.index, node.level)].rebalanced.load(std::memory_order_relaxed), "Rebalanced flag not set for rebalancing node");
        IndexType index = parent(node.index);
        for (IndexType level = node.level + 1; level <= logCapacityWithSuperRoot; ++level) {
            if (aggregates[aggregateIndex(index, level)].rebalanced.load(std::memory_order_relaxed)) {
                //Some thread rebalances this node which contains the given node
                //std::cout << "      Node " << node << " dominated by level " << level << ", node " << index << std::endl;
                aggregates[aggregateIndex(node.index, node.level)].rebalanced.store(false, std::memory_order_relaxed);
                return true;
            }
            index = parent(index);
        }
        //std::cout << "      Node " << node << " not dominated" << std::endl;
        return false;
    }

    /**
     * Returns the number of blocks in the given node as (level, index on the level).
     */
    SizeType numberOfBlocks(const RebalancingNode &node) const {
        if (node.level == logCapacityWithSuperRoot) {
            AssertMsg(node.index == 0, "Invalid index for super root.");
            return aggregates[aggregateIndex(0, node.level - 1)].numberOfBlocks;
        }
        return aggregates[aggregateIndex(node.index, node.level)].numberOfBlocks;
    }

    /**
     * Computes the range in the pma that a rebalancing node corresponds to.
     */
    Interval rebalancingRange(const RebalancingNode &node) const {
        if (node.level == logCapacityWithSuperRoot) {
            //Global rebalancing region, width depends on root node
            const SizeType numberOfBlocks = aggregates[aggregateIndex(0, logCapacity)].numberOfBlocks;
            const SizeType width = densityManager.minRangeLength(numberOfBlocks);
            return Interval(0, width);
        }
        const SizeType blocksPerAggregate = levelAggregateWidths[node.level] * AggregateWidth;
        const IndexType startIndex = node.index * blocksPerAggregate;
        const IndexType endIndex = (node.index + 1) * blocksPerAggregate;
        return Interval(startIndex, endIndex);
    }

    RebalancingTask getRebalancingTask(const RebalancingNode &node) const {
        return RebalancingTask{rebalancingRange(node), numberOfBlocks(node), node};
    }

    /**
     * Finds the extended pma index for the block with the given block number in
     * the rebalancing range indicated by the given rebalancing node.
     */
    ExtendedPmaIndex findBlockInRebalancingTask(const RebalancingNode &rootNode, SizeType blockNumber) const {
        SizeType remainingBlocks = blockNumber;
        RebalancingNode node = rootNode;
        if (node.level > logCapacity) {
            AssertMsg(node.index == 0, "Invalid index for super root");
            node.level = logCapacity;//If this is a super root rebalance, start at root.
        }
        if (blockNumber > aggregates[aggregateIndex(node)].numberOfBlocks) {
            //Block is beyond the end of this rebalancing task, return the end of this task
            return ExtendedPmaIndex(rebalancingRange(node).right, -1);
        }
        //std::cout << "         Searching for block number " << blockNumber << " from node " << rootNode << " with #blocks " << aggregates[aggregateIndex(node.index, node.level)].numberOfBlocks << std::endl;
        //Descend in the tree
        while (node.level > 0) {
            //Decide whether to descend to left or right child
            const SizeType leftChildBlocks = aggregates[aggregateIndex(leftChild(node.index), node.level - 1)].numberOfBlocks;
            if (remainingBlocks > leftChildBlocks) {
                //Left child does not have sufficient blocks, go right
                node.index = rightChild(node.index);
                remainingBlocks -= leftChildBlocks;
            } else {
                //Go left
                node.index = leftChild(node.index);
            }
            --node.level;
            AssertMsg(remainingBlocks <= aggregates[aggregateIndex(node)].numberOfBlocks, "Not enough blocks");
        }
        //On level 0, iterate over the blocks
        const Interval pmaRange = rebalancingRange(node);
        IndexType blockIndex = pmaRange.left;
        //std::cout << "         Lowest level: " << node << " looking for remaining blocks " << remainingBlocks << " in pma range " << pmaRange << " with number of blocks " << aggregates[aggregateIndex(node)].numberOfBlocks << std::endl;
        AssertMsg(remainingBlocks <= aggregates[aggregateIndex(node)].numberOfBlocks, "Not enough blocks");
        while (remainingBlocks >= 0) {
            if (remainingBlocks == 0) {
                //Found the block as pma block at pma index blockIndex
                AssertMsg(blockIndex <= pmaRange.right, "Went too far to the right.");
                return ExtendedPmaIndex(blockIndex, -1);
            }
            if (!sds->empty(blockIndex)) --remainingBlocks;
            if (remainingBlocks < sds->numberOfBufferBlocks(blockIndex)) {
                //The block is one of the buffer blocks
                return ExtendedPmaIndex(blockIndex, remainingBlocks);
            }
            remainingBlocks -= sds->numberOfBufferBlocks(blockIndex);
            ++blockIndex;
            AssertMsg(blockIndex <= pmaRange.right, "Went too far to the right.");
        }
        AssertMsg(false, "Should not reach this point.");
        return ExtendedPmaIndex(-1, -1);
    }

    IndexType lowestLevelNodeIndex(IndexType blockIndex) const {
        return blockIndex / AggregateWidth;
    }

private:
    /**
     * Returns the index of the parent of the given node on its level (not the absolute array-index).
     * Similarly, node is the number of the child node on its level.
     */
    IndexType parent(IndexType node) const {
        return node / 2;
    }

    /**
     * Returns the number of the left child of the given node.
     */
    IndexType leftChild(IndexType node) const {
        return node * 2;
    }


    /**
     * Returns the number of the right child of the given node.
     */
    IndexType rightChild(IndexType node) const {
        return node * 2 + 1;
    }

    /**
     * Computes the number of aggregate values in the given level.
     */
    SizeType numberOfAggregatesInLevel(IndexType level) const {
        AssertMsg(level <= logCapacityWithSuperRoot, "Not enough levels.");
        if (level == 0) {
            return capacity;
        }
        AssertMsg(capacity * AggregateWidth % levelAggregateWidths[level] == 0, "Round up needed");
        return capacity / levelAggregateWidths[level];
    }

    /**
     * Computes the index of the aggregate value with the given number in the given level in the
     * aggregates array.
     */
    IndexType aggregateIndex(IndexType aggregateNumber, IndexType level) const {
        AssertMsg(level <= logCapacityWithSuperRoot, "Not enough levels.");
        return levelStarts[level] + aggregateNumber;
    }

    /**
     * Computes the index of the aggregate value for the given node.
     */
    IndexType aggregateIndex(const RebalancingNode &node) const {
        return aggregateIndex(node.index, node.level);
    }

    /**
     * Computes the first block that is covered by the given aggregate on level zero.
     */
    IndexType getBlockIndexLevelZero(IndexType aggregateIndex) const {
        return aggregateIndex * AggregateWidth;
    }

    void recomputeLevelInfo() {
        levelStarts.resize(logCapacityWithSuperRoot + 1);
        levelAggregateWidths.resize(logCapacityWithSuperRoot + 1);
        IndexType currentStart = 0;
        SizeType currentWidth = capacity;
        SizeType currentAggregateWidth = 1;//On level 0, each aggregate corresponds to one aggregate on level 0.
        for (IndexType i = 0; i <= logCapacity; ++i) {
            levelStarts[i] = currentStart;
            levelAggregateWidths[i] = currentAggregateWidth;
            currentStart += currentWidth;
            currentWidth /= 2;
            currentAggregateWidth *= 2;
        }
        levelStarts[logCapacityWithSuperRoot] = currentStart;
        //The super root level covers any possible size
        levelAggregateWidths[logCapacityWithSuperRoot] = std::numeric_limits<SizeType>::max();
    }

    SizeType getSizeForCapacity(SizeType newCapacity) const {
        return static_cast<SizeType>(CapacityFactorForAllLevels * static_cast<double>(newCapacity));
    }

    SizeType aggregateNodeRangeLevelZero(IndexType nodeIndex) const {
        const IndexType effectivePmaSize = std::max(static_cast<IndexType>(1), static_cast<IndexType>(sds->size));
        SizeType aggregateNumberOfBlocks = 0;
        for (IndexType current = getBlockIndexLevelZero(nodeIndex); current < std::min(effectivePmaSize, getBlockIndexLevelZero(nodeIndex + 1)); ++current) {
            aggregateNumberOfBlocks += sds->empty(current) ? 0 : 1;
            aggregateNumberOfBlocks += sds->numberOfBufferBlocks(current);
        }
        return aggregateNumberOfBlocks;
    }

    SizeType aggregateNodeHigherLevel(IndexType index, IndexType level) const {
        //Sum over both children
        SizeType aggregateNumberOfBlocks = 0;
        AssertMsg(aggregateIndex(leftChild(index), level - 1) < 2 * capacity, "Invalid index");
        AssertMsg(aggregateIndex(rightChild(index), level - 1) < 2 * capacity, "Invalid index");
        aggregateNumberOfBlocks += aggregates[aggregateIndex(leftChild(index), level - 1)].numberOfBlocks;
        aggregateNumberOfBlocks += aggregates[aggregateIndex(rightChild(index), level - 1)].numberOfBlocks;
        return aggregateNumberOfBlocks;
    }

    void updateNode(IndexType index, IndexType level, bool needsRebalancing, SizeType newNumberOfBlocks) {
        //std::cout << "Updating node  " << index << " on level " << level << std::endl;
        AssertMsg(aggregateIndex(index, level) < 2 * capacity, "Invalid index");
        const bool withinThreshold = newNumberOfBlocks <= static_cast<IndexType>(densityManager.maxDensity(level, logCapacity) * static_cast<double>(AggregateWidth) * static_cast<double>(levelAggregateWidths[level]));
        const bool rebalanceNode = needsRebalancing && withinThreshold;
        const bool rebalanceParent = needsRebalancing && !withinThreshold;
        const SizeType oldValue = aggregates[aggregateIndex(index, level)].numberOfBlocks;
        //std::cout << "Recounting level 0, node " << recountingNode << ": new blocks " << newNumberOfBlocks << ", old: " << oldValue << " needs rebalancing: " << needsRebalancing << std::endl;

        AssertMsg(newNumberOfBlocks <= AggregateWidth * levelAggregateWidths[level] || needsRebalancing, "Blocks do not fit into node, but no rebalance requested.");
        if (newNumberOfBlocks != oldValue || !withinThreshold) {
            //std::cout << "Adding parent " << parent(index) << " to next level. Old value: " << oldValue << ", new: " << newNumberOfBlocks << ", blocks fit: " << blocksFit << std::endl;
            //Value changed or blocks do not fit; update and add parent for next level.
            aggregates[aggregateIndex(index, level)].numberOfBlocks = newNumberOfBlocks;
#if USE_BUCKET_MAP
            nextLevelRecountingNodes.insert(std::make_pair(parent(index), rebalanceParent));
#else
            ConcurrentMap::iterator it;
            bool inserted;
            std::tie(it, inserted) = nextLevelRecountingNodes.insert(std::make_pair(parent(index), rebalanceParent));
            if (!inserted) {
                //std::cout << "   Was already there" << std::endl;
                //There was alerady an entry with the given key, ensure that the mapped boolean is correct
                if (rebalanceParent) {
                    //std::cout << "      Set rebalance flag true" << std::endl;
                    //std::cout << "      Parent already there, set bool true" << std::endl;
                    //We might access this concurrently, but it is only ever overwritten from false to true, so nothing
                    //can go wrong here, even without atomics.
                    it->second = true;
                }
            }
#endif
        }
        if (rebalanceNode) {
            //std::cout << "Potentially adding rebalancing node level " << level << ", index " << index << std::endl;
            //Rebalance this node
            AssertMsg(!rebalanceParent, "Rebalancing child and parent");
            bool notRebalanced = false;
            const bool mustBeAdded = aggregates[aggregateIndex(index, level)].rebalanced.compare_exchange_strong(notRebalanced, true, std::memory_order_relaxed, std::memory_order_relaxed);
            if (mustBeAdded) {
                //std::cout << "   Added." << std::endl;
                AssertMsg(!notRebalanced, "Rebalanced twice.");
                sds->rebalancingNodes.emplace_back(level, index);
            } else {
                //std::cout << "   Not Added." << std::endl;
            }
        }
    }

    void updateHigherLevel(IndexType level) {
#if USE_BUCKET_MAP
        sds->recountingNodesMap.executeInParallel(numberOfThreads * HigherLevelParallelizationThresholdPerThread, [this, level](const std::pair<IndexType, bool> &item) {
            //std::cout << "   Updating node in level " << level << std::endl;
            const IndexType recountingNode = item.first;
            const bool needsRebalancing = item.second;
            SizeType aggregateNumberOfBlocks = aggregateNodeHigherLevel(recountingNode, level);
            updateNode(recountingNode, level, needsRebalancing, aggregateNumberOfBlocks);
        });
#else
        //std::cout << "Concurrent map total size is " << sds->recountingNodesVector.size() << std::endl;
#pragma omp parallel for schedule(static) num_threads(numberOfThreads) if(sds->recountingNodesVector.size() > numberOfThreads * HigherLevelParallelizationThresholdPerThread)
        for (auto const &[recountingNode, needsRebalancing] : sds->recountingNodesVector) {
            //std::cout << "   Updating node in level " << level << std::endl;
            SizeType aggregateNumberOfBlocks = aggregateNodeHigherLevel(recountingNode, level);
            updateNode(recountingNode, level, needsRebalancing, aggregateNumberOfBlocks);
        }
#endif
    }

    void updateLevelZero() {
#if USE_BUCKET_MAP
        sds->recountingNodesMap.executeInParallel(numberOfThreads * LevelZeroParallelizationThresholdPerThread, [this](const std::pair<IndexType, bool> &item) {
            //std::cout << "   Updating node in level 0" << std::endl;
            const IndexType recountingNode = item.first;
            const bool needsRebalancing = item.second;
            SizeType aggregateNumberOfBlocks = aggregateNodeRangeLevelZero(recountingNode);
            updateNode(recountingNode, 0, needsRebalancing, aggregateNumberOfBlocks);
        });
#else
        //std::cout << "Concurrent map total size is " << sds->recountingNodesVector.size() << std::endl;
#pragma omp parallel for schedule(static) num_threads(numberOfThreads) if (sds->recountingNodesVector.size() > numberOfThreads * LevelZeroParallelizationThresholdPerThread)
        for (auto const &[recountingNode, needsRebalancing] : sds->recountingNodesVector) {
            //std::cout << "   Updating node in level 0" << std::endl;
            SizeType aggregateNumberOfBlocks = aggregateNodeRangeLevelZero(recountingNode);
            updateNode(recountingNode, 0, needsRebalancing, aggregateNumberOfBlocks);
        }
#endif
    }

public:
    void validate(SizeType sdsSize) const {
        //For level 0:
        for (IndexType i = 0; i < numberOfAggregatesInLevel(0); ++i) {
            SizeType aggregateNumberOfBlocks = 0;
            for (IndexType current = getBlockIndexLevelZero(i); current < std::min(sdsSize, getBlockIndexLevelZero(i + 1)); ++current) {
                aggregateNumberOfBlocks += sds->empty(current) ? 0 : 1;
                aggregateNumberOfBlocks += sds->numberOfBufferBlocks(current);
            }
            AssertMsg(i < 2 * capacity, "Invalid index");
            AssertMsg(aggregateNumberOfBlocks == aggregates[aggregateIndex(i, 0)].numberOfBlocks, "Level 0: Aggregate number of blocks " << i << " is wrong, has " << aggregates[aggregateIndex(i, 0)].numberOfBlocks << ", should be " << aggregateNumberOfBlocks << ".");
        }
        //For the other levels:
        for (IndexType level = 1; level <= logCapacity; ++level) {
            for (IndexType i = 0; i < numberOfAggregatesInLevel(level); ++i) {
                //Sum over both children
                SizeType aggregateNumberOfBlocks = 0;
                AssertMsg(aggregateIndex(leftChild(i), level - 1) < 2 * capacity, "Invalid index");
                AssertMsg(aggregateIndex(rightChild(i), level - 1) < 2 * capacity, "Invalid index");
                aggregateNumberOfBlocks += aggregates[aggregateIndex(leftChild(i), level - 1)].numberOfBlocks;
                aggregateNumberOfBlocks += aggregates[aggregateIndex(rightChild(i), level - 1)].numberOfBlocks;
                //Check the value
                AssertMsg(aggregateIndex(i, level) < 2 * capacity, "Invalid index");
                AssertMsg(aggregateNumberOfBlocks == aggregates[aggregateIndex(i, level)].numberOfBlocks, "Level " << level << ": Aggregate number of blocks " << i << " is wrong, has " << aggregates[aggregateIndex(i, level)].numberOfBlocks << ", should be " << aggregateNumberOfBlocks << ".");
            }
        }
    }

    bool nodeFitsAllBlocks(const RebalancingNode &node) const {
        if (node.level > logCapacity) return true;//Global resize, always fits.
        const bool blocksFit = aggregates[aggregateIndex(node.index, node.level)].numberOfBlocks <= levelAggregateWidths[node.level];
        AssertMsg(blocksFit, "Invalid rebalancing node!");
        return blocksFit;
    }
};
}
