#pragma once

#include <iostream>
#include <memory>
#include <omp.h>

#include "helpers.hpp"
#include "assert.hpp"
#include "profiler.hpp"
#include "debugStream.hpp"
#include "blockSizeManager.hpp"
#include "typedefs.hpp"
#include "iterators.hpp"

#include "overallocatedBlock.hpp"

namespace DataStructures {
template<typename KEY, typename VALUE, KEY NULL_KEY, typename KEY_OF, typename KEY_COMPARE = std::less<KEY>, typename ALLOCATOR = std::allocator<VALUE>, UnsignedSizeType MAX_NUMBER_OF_THREADS = 4, BlockSizeType BLOCK_SIZE = 32, bool ALLOW_IN_PLACE_INSERTS = true, typename PROFILER = Helpers::Profiler::NoProfiler, typename DEBUG_STREAM = Helpers::NoDebugStream<KEY>>
class OverallocatedBatchedSparseArray {
private:
    //Size configuration
    static constexpr SizeType LogInitialArrayCapacity = 12;//The initial capacity (in blocks). Must be a power of 2.
    static constexpr SizeType InitialArrayCapacity = 1LL << LogInitialArrayCapacity;//The initial capacity (in blocks). Must be a power of 2.
    static constexpr BlockSizeType BlockSize = BLOCK_SIZE;//Number of elements in each block, must be divisible by 2.
    static constexpr BlockSizeType MinBlockSize = BlockSize / 2;//Minimum number of elements that each block has, unless there is only one block. Must not be greater than BlockSize / 2.

    //Configuration of the operations
    static constexpr UnsignedSizeType MaxNumberOfThreads = MAX_NUMBER_OF_THREADS;
    static constexpr SizeType ParallelPrefixSumMinimum = 8 * MaxNumberOfThreads;
    static constexpr SizeType BinarySearchCutoff = 100;//Minimum size of an interval such that binary search is used. Below, use linear search.
    //static constexpr SizeType FasterMergeMinimum = 2;//Minimum number of elements inserted into a block such that faster inline merging is used instead of repeated insertions.
    static constexpr bool AllowInPlaceInserts = ALLOW_IN_PLACE_INSERTS;//If true, will try to insert batches without rewriting all blocks to a new array whenever possible
    static constexpr bool UsePrecalculatedInsertionLengths = true;//Default: true. If true, will count the number of elements that go into a block before inserting them to decide if a split will ever be necessary before inserting.
    static constexpr double InitialOverallocationFactor = 1.8;//Factor of extra width that is used during a global rewriting operation.

    //Debugging configuration
    using DebugStream = DEBUG_STREAM;
    using Profiler = PROFILER;
    static constexpr bool Debug = false;//Default: false
    static constexpr bool Profile = false;//Default: false
    static constexpr bool ProfileConcurrent = Profile && std::is_same_v<PROFILER, Helpers::Profiler::ConcurrentProfiler>;

public:
    //ValueType is the type of objects stored in the blocks.
    //KeyType is the type that is used for comparison. It is stored in the packed memory array.
    //KeyOf offers a KeyOf::keyOfValue-function that extracts the key from a value.
    //KeyCompare offers an access operator (left, right), that compares two keys.
    using ValueType = VALUE;
    using Reference = ValueType&;
    using Pointer = ValueType*;
    using ConstReference = ValueType const&;
    using KeyType = KEY;
    using KeyCompare = KEY_COMPARE;
    using KeyOf = KEY_OF;
    using Allocator = ALLOCATOR;
    using AllocatorTraits = std::allocator_traits<Allocator>;
    using SdsType = OverallocatedBatchedSparseArray<KeyType, ValueType, NULL_KEY, KeyOf, KeyCompare, Allocator, MaxNumberOfThreads, BlockSize, AllowInPlaceInserts, Profiler, DebugStream>;
    using iterator = BlockSparseArrayIterator<SdsType>;

    friend class BlockSparseArrayIterator<SdsType>;

    SizeType FasterMergeMinimum = 3;//Minimum number of elements inserted into a block such that faster inline merging is used instead of repeated insertions.
    static constexpr bool UseInverseMerge = true;//Default: true. If true, will distribute the work of merged tasks to the tasks that are involved. Otherwise, one task does all the work.

private:
    using BlockSizeManager = DataStructures::HierarchicalBlockSizeManager<SdsType>;
    //using BlockSizeManager = DataStructures::LinearBlockSizeManager<SdsType>;
    using PmaEntryType = OverallocatedPmaEntry<KeyType, ValueType, NULL_KEY, KeyOf, KeyCompare, Allocator, MinBlockSize, BlockSize, SdsType>;
    using PmaEntryReference = PmaEntryType&;
    using PmaEntryPointer = PmaEntryType* ;

public:
    SizeType capacity;//The capacity of the pma
    SizeType swapCapacity;//The capacity of the swap pma
    SizeType size;//End of the region of entries that might be used
    double overallocationFactor;
    [[no_unique_address]] KeyCompare comp;
    //The actual data
    std::unique_ptr<PmaEntryType[]> pma;
    std::unique_ptr<PmaEntryType[]> swapPma;
    BlockSizeManager sizeManager;
    alignas(64) std::array<TaskInfo, MaxNumberOfThreads + 1> tasks;// + 1 as sentinel
    [[no_unique_address]] Allocator allocator;
    //For debugging and profiling.
    [[no_unique_address]] Profiler profiler;
    [[no_unique_address]] DebugStream debugStream;

public:
    OverallocatedBatchedSparseArray(Allocator &alloc, UnsignedIndexType numThreads) :
        capacity(InitialArrayCapacity),
        swapCapacity(InitialArrayCapacity),
        size(0),
        overallocationFactor(InitialOverallocationFactor),
        comp(),
        sizeManager(this),
        allocator(alloc) {
        //Allocate the necessary space and initialize it to all zero.
        pma = std::make_unique<PmaEntryType[]>(capacity);
        swapPma = std::make_unique<PmaEntryType[]>(swapCapacity);
        (void) numThreads;
    }

    ~OverallocatedBatchedSparseArray() {
        for (IndexType i = 0; i < size; ++i) {
            if (!pma[i].isEmpty()) {
                pma[i].deleteBlock(allocator);
            }
        }
    }

public:
    void insertBatch(Pointer batch, SizeType batchSize, UnsignedSizeType numberOfThreads) {
        AssertMsg(batchSorted(batch, batchSize), "Batch not sorted.");
        AssertMsg(numberOfThreads <= MaxNumberOfThreads, "Too many threads.");
        AssertMsg(batchSize >= static_cast<SizeType>(numberOfThreads), "Batch is too small.");
        if constexpr (Debug) std::cout << "Distributing a batch of size " << batchSize << " with " << numberOfThreads << " tasks." << std::endl;
        //1. Find the block corresponding to the splitter using binary search and compute the required widths.
        const SizeType splitterDistance = batchSize / static_cast<SizeType>(numberOfThreads);
        //Compute the last task info as sentinel
        tasks[numberOfThreads].blockForBatchSplitter = size;//sentinel, as above for the end
        tasks[numberOfThreads].firstBatchElement = batchSize;//Sentinel for the last task
#pragma omp parallel num_threads(numberOfThreads) default(none) shared(sizeManager, tasks) firstprivate(numberOfThreads, size, batchSize, splitterDistance, batch)
        {
            const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
            tasks[thread].blockForBatchSplitter = findBlockIndex(KeyOf::keyOfValue(batch[thread * static_cast<UnsignedIndexType>(splitterDistance)]));
            tasks[thread].firstBatchElement = findFirstBatchElement(batch, static_cast<IndexType>(thread) * splitterDistance, head(tasks[thread].blockForBatchSplitter));
            IndexType nextBlockForBatchSplitter, nextFirstBatchElement;
            if (thread == numberOfThreads - 1) {
                nextBlockForBatchSplitter = size;
                nextFirstBatchElement = batchSize;
            } else {
                nextBlockForBatchSplitter = findBlockIndex(KeyOf::keyOfValue(batch[(thread + 1) * static_cast<UnsignedIndexType>(splitterDistance)]));
                nextFirstBatchElement = findFirstBatchElement(batch, static_cast<IndexType>(thread + 1) * splitterDistance, head(nextBlockForBatchSplitter));
            }

            const SizeType numberOfOldElements = sizeManager.getNumberOfElements(tasks[thread].blockForBatchSplitter, nextBlockForBatchSplitter);
            const SizeType numberOfNewElements = nextFirstBatchElement - tasks[thread].firstBatchElement;
            tasks[thread].requiredWidth = maxRedistributionWidth(numberOfOldElements, numberOfNewElements);
        }

        //2. Sequentially fix the first element in the batch if necessary. This avoids more than one tasks working on a single block.
        // Also accumulate the required widths. Must not be parallelized!
        tasks[0].firstBatchElement = 0;
        tasks[0].type = TaskType::INSERTION;//default
        SizeType mergedTasks = 0;
        bool hasMergedTask = false;
        for (UnsignedIndexType thread = 1; thread < numberOfThreads; ++thread) {
            if (tasks[thread].blockForBatchSplitter == tasks[thread - 1].blockForBatchSplitter) {
                //If (at least) two tasks start at the first block, make sure the last of those task does all the work for this block
                tasks[thread].firstBatchElement = tasks[thread - 1].firstBatchElement;
                tasks[thread].type = TaskType::INSERTION;
                if constexpr (UseInverseMerge) {
                    tasks[thread - 1].type = TaskType::INVERSE_MERGE;
                    hasMergedTask = true;
                    ++mergedTasks;
                    tasks[thread].mergedTasks = mergedTasks;
                } else {
                    tasks[thread].requiredWidth += tasks[thread - 1].requiredWidth;
                }
                tasks[thread - 1].requiredWidth = 0;
            } else {
                if constexpr (UseInverseMerge) {
                    tasks[thread].type = TaskType::INSERTION;
                    tasks[thread].mergedTasks = 0;
                    mergedTasks = 0;
                }
            }
        }

        if constexpr (UseInverseMerge) {
            distributeWorkInMergedTasks(batch, batchSize, numberOfThreads);
        }

        //Decide globally whether we can execute all tasks without writing to the swap data structures. Could be parallelized (reduction), but not faster
        bool useInPlaceInserts = AllowInPlaceInserts;
        for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread) {
            if (useInPlaceInserts) {
                const SizeType currentWidth = tasks[thread + 1].blockForBatchSplitter - tasks[thread].blockForBatchSplitter;
                if (tasks[thread].requiredWidth > currentWidth || (UseInverseMerge && tasks[thread].type == TaskType::INVERSE_MERGE)) {
                    //This task could not be executed in its interval, we need a global rewriting operation
                    useInPlaceInserts = false;
                }
            }
        }
        tasks[numberOfThreads].requiredWidth = 0;//Sentinel
#ifndef NDEBUG
        validateTaskDistribution(numberOfThreads, batchSize, useInPlaceInserts);
#endif

        //4. Here, we know whether we can insert in-place or need a global rewriting operation
        if (useInPlaceInserts) {
            if constexpr (Profile) profiler.countEvent(Helpers::Profiler::Event::BATCH_INSERT_IN_PLACE);
            //A-1. Start all the tasks with output-interval = input-interval (parallel)
            //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) firstprivate(batch)
            {
                const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
                startTaskInPlace(thread, batch);
            }
#ifndef NDEBUG
            sizeManager.validate();
#endif
        } else {
            if constexpr (Profile) profiler.countEvent(Helpers::Profiler::Event::BATCH_INSERT_OUT_OF_PLACE);
            //B-1. Compute prefix sum over task widths to get start indices
            SizeType prefixWidth = tasks[0].blockForBatchSplitter;//Start writing only after the blocks that are not touched
            for (UnsignedIndexType i = 0; i <= numberOfThreads; ++i) {
                tasks[i].startIndex = prefixWidth;
                if constexpr (AllowInPlaceInserts) {
                    prefixWidth += static_cast<SizeType>(overallocationFactor * static_cast<double>(tasks[i].requiredWidth));
                } else {
                    prefixWidth += tasks[i].requiredWidth;//Do not get extra space if it is not needed.
                }
                if constexpr (Debug) std::cout << "   Task " << i << ": " << tasks[i] << std::endl;
            }

            //B-2 Enlarge the data structure as needed
            increaseSize(prefixWidth);

            //B-3 Start the insertions in the respective intervals
            //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) firstprivate(batch, batchSize)
            {
                const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
                if constexpr (Debug) debugStream.add("Thread starting task " + std::to_string(thread));
                startTaskOutOfPlace(thread, batch, batchSize);
            }
            if constexpr (UseInverseMerge) {
                if (hasMergedTask) {
                    postprocessInverseMergeTasks(numberOfThreads);
                }
            }
            //B-4 Write the block back to the old memory
            //If needed, reallocate a larger array
            resizeMainPma();
            //Then, write all blocks back in parallel, use the actual number of tasks to distribute them uniformly in the array
            //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none)
            {
                const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
                startRewritingTask(thread);
            }

            sizeManager.recompute(numberOfThreads);
        }
    }

    void removeBatch(Pointer batch, SizeType batchSize, UnsignedIndexType numThreads) {
        (void) batch;
        (void) batchSize;
        (void) numThreads;
    }

    bool contains(const KeyType e) {
        IndexType blockIndex = findContainingBlock(e);
        bool result = blockIndex >= 0 && !pma[blockIndex].isEmpty() && pma[blockIndex].contains(e);
        return result;
    }

    iterator find(const KeyType e) {
        IndexType blockIndex = findContainingBlock(e);
        if (blockIndex < 0 || pma[blockIndex].isEmpty()) {
            return end();
        }
        const BlockIndexType indexInBlock = pma[blockIndex].findIndex(e);
        if (indexInBlock >= blockSize(blockIndex) || KeyOf::keyOfValue(pma[blockIndex].get(indexInBlock)) != e) {
            return end();
        }
        return iterator(this, blockIndex, indexInBlock);
    }

    /**
     * Returns an iterator to the first entry whose key does not satisfy
     * comp(KeyOf::keyOfValue(entry), e).
     */
    iterator lowerBound(const KeyType e) {
        IndexType blockIndex = findBlockIndex(e);
        if (blockIndex >= size) {
            //The key is larger than all elements in the data structure.
            return end();
        }
        const BlockIndexType indexInBlock = pma[blockIndex].findIndex(e);
        if (indexInBlock >= blockSize(blockIndex)) {
            //The element is larger than all elements in the block, return iterator to the start of the next block.
            blockIndex = findNext(blockIndex, size);
            if (blockIndex >= size) {
                //There is no further block.
                return end();
            }
            AssertMsg(!empty(blockIndex), "Returning an invalid iterator.");
            return iterator(this, blockIndex, 0);
        }
        return iterator(this, blockIndex, indexInBlock);
    }

    template<typename EVALUATOR>
    void rangeQuery(KeyType left, KeyType right, EVALUATOR f) {
        iterator current = lowerBound(left);

        while (current != end() && comp(KeyOf::keyOfValue(*current), right)) {
            AssertMsg((comp(left, KeyOf::keyOfValue(*current)) || left == KeyOf::keyOfValue(*current)) && comp(KeyOf::keyOfValue(*current), right), "Range query considers an invalid value");
            f(*current);
            ++current;
        }
    }

private:
    /**
     * Inserts the range batchRange of the given batch, which corresponds to pmaRange
     * in the PMA into that same interval of the PMA.
     * Requires that the new blocks written for that operation fit into the pmaRange.
     */
    void insertBatchRange(Pointer batch, const Interval &batchRange, const Interval &pmaRange) {
        IndexType batchIndex = batchRange.left;
        IndexType blockIndex = pmaRange.left;
        IndexType nextBlockIndex;
        while (batchIndex < batchRange.right) {
            //Use exponential search to find the block that the next batch element goes into
            blockIndex = exponentialSearch(KeyOf::keyOfValue(batch[batchIndex]), blockIndex, pmaRange.right);
            nextBlockIndex = findNext(blockIndex, pmaRange.right);
            AssertMsg(blockIndex == linearSearch(KeyOf::keyOfValue(batch[batchIndex]), blockIndex, pmaRange.right), "Wrong exponential search results");
            insertElementsIntoBlock(blockIndex, nextBlockIndex, pmaRange, pmaRange, batch, batchIndex, batchRange);
        }
        AssertMsg(batchIndex == batchRange.right, "Entire batch was inserted.");
    }

    /**
     * Inserts the range batchRange of the given batch, which corresponds to pmaInputRange
     * in the PMA into the range pmaOutputRange of the PMA.
     *
     * Returns the number of blocks that was written to swapPma.
     */
    SizeType insertBatchRange(Pointer batch, const Interval &batchRange, const Interval &pmaInputRange, const Interval &pmaOutputRange) {
        IndexType batchIndex = batchRange.left;
        IndexType blockIndex = pmaInputRange.left;//Start the insertion at the beginning of the input range.
        AssertMsg(pmaInputRange.length() == 0 || !empty(blockIndex), "Input batch starts on an empty block");
        IndexType nextBlockIndex = findNext(blockIndex, pmaInputRange.right);//The next non-empty block
        IndexType outputBlockNumber = 0;//Keep track of which block we are in to calculate the write indices.

        //Linearly walk over the input range. If necessary, insert the respective batch elements into the block. Then, write it to the next position in the output range (even if nothing was inserted).
        while (blockIndex < pmaInputRange.right) {
            if (batchIndex < batchRange.right && (comp(KeyOf::keyOfValue(batch[batchIndex]), head(nextBlockIndex)) || empty(nextBlockIndex))) {
                AssertMsg(blockIndex < pmaInputRange.right, "Inserting into block outside of the given pmaInputRange.");
                outputBlockNumber = insertElementsIntoBlock<true>(blockIndex, nextBlockIndex, pmaInputRange, pmaOutputRange, batch, batchIndex, batchRange, outputBlockNumber);
            } else {
                //No inputs needed, simply output the block to its respective position.
                writeBlock(blockIndex, pmaOutputRange.left, outputBlockNumber);
                ++outputBlockNumber;
            }
            //Advance nextBlockIndex and blockIndex by one block
            blockIndex = nextBlockIndex;
            nextBlockIndex = findNext(nextBlockIndex, pmaInputRange.right);
        }
        if (pmaInputRange.length() == 0) {
            outputBlockNumber = insertIntoEmptyRange(batch, batchRange, batchIndex, pmaOutputRange, outputBlockNumber);
        } else {
            AssertMsg(batchIndex == batchRange.right, "Entire batch was inserted.");
        }
        return outputBlockNumber;
    }

    /**
     * Returns the number of the next block to be written to the pma.
     */
    template<bool WRITE_TO_SWAP = false>
    IndexType insertElementsIntoBlock(IndexType blockIndex, IndexType nextBlockIndex, const Interval &pmaInputRange, const Interval &pmaOutputRange, Pointer batch, IndexType &batchIndex, Interval batchRange, IndexType outputBlockNumber = 0) {
        if constexpr (UsePrecalculatedInsertionLengths) {
            return insertElementsIntoBlockWithPrecalculatedInsertionWidths<WRITE_TO_SWAP>(blockIndex, nextBlockIndex, pmaInputRange, pmaOutputRange, outputBlockNumber, batch, batchIndex, batchRange);
        } else {
            return insertElementsIntoBlockWithoutPrecalculatedInsertionWidths<WRITE_TO_SWAP>(blockIndex, nextBlockIndex, pmaOutputRange, outputBlockNumber, batch, batchIndex, batchRange);
        }
    }

    /**
     * Returns the number of the next block to be written to the pma.
     */
    template<bool WRITE_TO_SWAP>
    [[nodiscard]] IndexType insertElementsIntoBlockWithPrecalculatedInsertionWidths(IndexType blockIndex, IndexType nextBlockIndex, const Interval &pmaInputRange, const Interval &pmaOutputRange, IndexType outputBlockNumber, Pointer batch, IndexType &batchIndex, Interval batchRange) {
        const IndexType lastBlockElement = findLastBatchElementForBlock(batch, batchIndex, batchRange, nextBlockIndex, pmaInputRange);
        const SizeType elementsInBlock = lastBlockElement - batchIndex;
        //Check if these elements fit into the block
        AssertMsg(elementsInBlock > 0, "Inserting 0 elements into a block");
        if (blockSize(blockIndex) + elementsInBlock <= BlockSize) {
            //There is enough space in the block. Merge the elements into the block directly.
            insertIntoBlockWithoutSplit(blockIndex, batch, batchIndex, lastBlockElement);
            if constexpr (WRITE_TO_SWAP) {
                writeBlock(blockIndex, pmaOutputRange.left, outputBlockNumber);
                ++outputBlockNumber;
            }
        } else {
            outputBlockNumber = insertIntoBlockWithSplit<WRITE_TO_SWAP>(blockIndex, batch, batchIndex, lastBlockElement, pmaOutputRange, outputBlockNumber);
        }
        return outputBlockNumber;
    }

    /**
     * Returns the number of the next block to be written to the pma.
     */
    template<bool WRITE_TO_SWAP>
    [[nodiscard]] IndexType insertElementsIntoBlockWithoutPrecalculatedInsertionWidths(IndexType blockIndex, IndexType nextBlockIndex, const Interval &pmaOutputRange, IndexType outputBlockNumber, Pointer batch, IndexType &batchIndex, const Interval &batchRange) {
        //Read the insertion block from the pma
        IndexType insertionBlockIndex = blockIndex;
        PmaEntryType insertionBlock = pma[blockIndex];
        bool notYetSplit = true;
        if constexpr (WRITE_TO_SWAP) {
            pma[blockIndex] = PmaEntryType();
        }
        //Keep the one (!) block that might remain when splitting a block.
        PmaEntryType rightHalf(nullptr, NULL_KEY, 0);
        //Insert all required batch elements into the block, split it if necessary.
        BlockSizeType lastInsertionIndex = 0;//Hint for the search
        while (batchIndex < batchRange.right && (comp(KeyOf::keyOfValue(batch[batchIndex]), head(nextBlockIndex)) || empty(nextBlockIndex))) {
            //Check if we need to switch the insertion block to a previously split right half
            if (!rightHalf.isEmpty() && (comp(rightHalf.head, KeyOf::keyOfValue(batch[batchIndex])) || KeyOf::keyOfValue(batch[batchIndex]) == rightHalf.head)) {
                AssertMsg(!notYetSplit, "Not allowed to split here.");
                //Write the current insertionBlock, switch it to rightHalf.
                if constexpr(WRITE_TO_SWAP) {
                    writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
                    ++outputBlockNumber;
                } else {
                    insertionBlockIndex = 1 + writeBlockInPlace(insertionBlock, insertionBlockIndex, pmaOutputRange);
                }
                insertionBlock = rightHalf;
                rightHalf = PmaEntryType();
                lastInsertionIndex = 0;
            }
            //Insert the next element into insertionBlock
            if (insertionBlock.size < BlockSize) {
                //There is still space, insert another element
                std::tie(std::ignore, lastInsertionIndex) = insertionBlock.insertWithIndex(batch[batchIndex], lastInsertionIndex);
                ++batchIndex;
            } else {
                if (notYetSplit) {
                    if (!WRITE_TO_SWAP) {
                        //Set the block size to 0 in the size manager, will be reset correctly later.
                        sizeManager.adjustBlockSize(insertionBlockIndex, insertionBlock.size, 0);
                    }
                    notYetSplit = false;
                }
                //Not enough space left, split the block
                PmaEntryType newBlock;
                newBlock = insertionBlock.split(allocator);
                if (!rightHalf.isEmpty()) {
                    //There was already a block split off. We know that the next insertion can no longer go to insertion block.
                    //Write it and switch to newBlock
                    if constexpr(WRITE_TO_SWAP) {
                        writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
                        ++outputBlockNumber;
                    } else {
                        insertionBlockIndex = 1 + writeBlockInPlace(insertionBlock, insertionBlockIndex, pmaOutputRange);
                    }
                    insertionBlock = newBlock;
                } else {
                    //Keep newBlock as rightHalf.
                    rightHalf = newBlock;
                }
                lastInsertionIndex = 0;
                AssertMsg(!rightHalf.isEmpty(), "Split failed");
                //Continue: Will retry for the current element and insert it into either insertionBlock or rightHalf.
                continue;
            }
        }
        //Write insertion block
        if (notYetSplit) {
            if (WRITE_TO_SWAP) {
                writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
                ++outputBlockNumber;
            } else {
                //The block was never split, we do not need to rewrite it. Only update the size manager.
                sizeManager.adjustBlockSize(insertionBlockIndex, 0, insertionBlock.size);
            }
        } else {
            if constexpr(WRITE_TO_SWAP) {
                writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
                ++outputBlockNumber;
            } else {
                insertionBlockIndex = 1 + writeBlockInPlace(insertionBlock, insertionBlockIndex, pmaOutputRange);
            }
        }
        if (!rightHalf.isEmpty()) {
            AssertMsg(!notYetSplit, "Invalid status.");
            //Write remaining right half to the output, too.
            if constexpr(WRITE_TO_SWAP) {
                writeBlock(rightHalf, pmaOutputRange.left, outputBlockNumber);
                ++outputBlockNumber;
            } else {
                insertionBlockIndex = 1 + writeBlockInPlace(rightHalf, insertionBlockIndex, pmaOutputRange);
            }
        }
        return outputBlockNumber;
    }

    [[nodiscard]] IndexType findLastBatchElementForBlock(Pointer batch, IndexType batchIndex, const Interval &batchRange, IndexType nextBlockIndex, const Interval &pmaRange) const {
        //Find the last element from the batch that goes into the current block
        if (nextBlockIndex == pmaRange.right) {
            return batchRange.right;
        }
        IndexType lastBlockElement = batchIndex;
        while (lastBlockElement < batchRange.right && comp(KeyOf::keyOfValue(batch[lastBlockElement]), head(nextBlockIndex))) {
            ++lastBlockElement;
        }
        return lastBlockElement;
    }

    void insertIntoBlockWithoutSplit(IndexType blockIndex, Pointer batch, IndexType &batchIndex, IndexType lastBlockElement) {
        const BlockSizeType oldSize = blockSize(blockIndex);
        const BlockSizeType numberOfInsertions = static_cast<BlockSizeType>(lastBlockElement - batchIndex);
        if (numberOfInsertions >= FasterMergeMinimum) {
            pma[blockIndex].mergeFaster(batch + batchIndex, numberOfInsertions);
        } else {
            pma[blockIndex].merge(batch + batchIndex, numberOfInsertions);
        }
        sizeManager.adjustBlockSize(blockIndex, oldSize, blockSize(blockIndex));
        batchIndex = lastBlockElement;
    }

    /**
     * Returns the number of the next block to be written to the pma.
     */
    template<bool WRITE_TO_SWAP = false>
    [[nodiscard]] IndexType insertIntoBlockWithSplit(const IndexType blockIndex, const Pointer batch, IndexType &batchIndex, const IndexType lastBlockElement, const Interval &pmaOutputRange, IndexType outputBlockNumber) {
        //The elements that need to go into this block do not fit here.
        //Merge until the block is full, then split the upper half of the  elements of the block into a new block
        //and continue merging there. Repeat until all elements from the batch that go into the block are merged.
        IndexType insertionBlockIndex = blockIndex;
        PmaEntryType insertionBlock = pma[blockIndex];
        BlockIndexType lastInsertionIndex = 0;
        sizeManager.adjustBlockSize(blockIndex, insertionBlock.size, 0);
        pma[blockIndex] = PmaEntryType();
        PmaEntryType rightHalf = PmaEntryType();
        while (batchIndex < lastBlockElement) {
            if (!rightHalf.isEmpty() && (comp(rightHalf.head, KeyOf::keyOfValue(batch[batchIndex])) || KeyOf::keyOfValue(batch[batchIndex]) == rightHalf.head)) {
                //Continue to the right, we can write the left half to the output
                if constexpr (WRITE_TO_SWAP) {
                    writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
                    ++outputBlockNumber;
                } else {
                    insertionBlockIndex = 1 + writeBlockInPlace(insertionBlock, insertionBlockIndex, pmaOutputRange);
                }
                insertionBlock = rightHalf;
                rightHalf = PmaEntryType();
                lastInsertionIndex = 0;
            }
            if (insertionBlock.size < BlockSize) {
                std::tie(std::ignore, lastInsertionIndex) = insertionBlock.insertWithIndex(batch[batchIndex], lastInsertionIndex);
                ++batchIndex;
            } else {
                //Not enough space left, split the block
                PmaEntryType newBlock;
                newBlock = insertionBlock.split(allocator);
                if (!rightHalf.isEmpty()) {
                    //There was already a block split off. We know that the next insertion can no longer go to insertion block.
                    //Write it and switch to newBlock
                    if constexpr (WRITE_TO_SWAP) {
                        writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
                        ++outputBlockNumber;
                    } else {
                        insertionBlockIndex = 1 + writeBlockInPlace(insertionBlock, insertionBlockIndex, pmaOutputRange);
                    }
                    insertionBlock = newBlock;
                    lastInsertionIndex = 0;
                } else {
                    //Keep newBlock as rightHalf.
                    rightHalf = newBlock;
                }
            }
        }
        //Write insertion block
        if constexpr(WRITE_TO_SWAP) {
            writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
            ++outputBlockNumber;
        } else {
            insertionBlockIndex = 1 + writeBlockInPlace(insertionBlock, insertionBlockIndex, pmaOutputRange);
        }
        if (!rightHalf.isEmpty()) {
            //Write remaining right half to the output, too.
            if constexpr(WRITE_TO_SWAP) {
                writeBlock(rightHalf, pmaOutputRange.left, outputBlockNumber);
                ++outputBlockNumber;
            } else {
                insertionBlockIndex = 1 + writeBlockInPlace(rightHalf, insertionBlockIndex, pmaOutputRange);
            }
        }
        return outputBlockNumber;
    }

    /**
     * Returns the number of the next block to be written to the pma.
     */
    [[nodiscard]] IndexType insertIntoEmptyRange(Pointer batch, const Interval &batchRange, IndexType &batchIndex, const Interval &pmaOutputRange, IndexType outputBlockNumber) {
        //The pma range was empty, we need to create all new blocks
        PmaEntryType insertionBlock = PmaEntryType();
        insertionBlock.allocateBlock(allocator);
        while (batchIndex < batchRange.right) {
            if (insertionBlock.size == BlockSize) {
                //Block is full, write it to output and get a new one
                PmaEntryType newBlock{};
                SizeType remainingElements = batchRange.right - batchIndex;
                if (remainingElements > 0 && remainingElements < MinBlockSize) {
                    //The remaining elements would not be sufficient to fill the next block, split the last one
                    newBlock = insertionBlock.split(allocator);
                } else {
                    newBlock.allocateBlock(allocator);
                }
                writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
                ++outputBlockNumber;
                insertionBlock = newBlock;
                if (batchIndex >= batchRange.right) {
                    batchIndex = batchRange.right;
                    break;
                }
            }
            //Insert as many elements into the block as possible
            IndexType numberOfInsertions = std::min(static_cast<IndexType>(BlockSize), batchRange.right - batchIndex);
            insertionBlock.mergeFaster(batch + batchIndex, static_cast<BlockSizeType>(numberOfInsertions));
            batchIndex += numberOfInsertions;
        }
        if (insertionBlock.size > 0) {
            writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
            ++outputBlockNumber;
        } else {
            insertionBlock.deleteBlock(allocator);
        }
        AssertMsg(batchIndex == batchRange.right, "Entire batch was inserted.");
        return outputBlockNumber;
    }

    /**
     * This is currently not used. Can be used as part of an interface that does not require presorted,
     * duplicate-free batches.
     */
    void deduplicate(Pointer &batch, SizeType &batchSize) {
        //Copy the batch to keep it in its previous shape if it is ever reused.
        Pointer newBatch = static_cast<Pointer>(malloc(static_cast<size_t>(batchSize) * sizeof(ValueType)));
        std::copy(batch, batch + batchSize, newBatch);
        auto newEnd = std::unique(newBatch, newBatch + batchSize, [](const ValueType &lhs, const ValueType &rhs) -> bool {
            return KeyOf::keyOfValue(lhs) == KeyOf::keyOfValue(rhs);
        });
        batch = newBatch;
        batchSize = static_cast<SizeType>(newEnd - batch);
    }

    [[nodiscard]] IndexType inverseMerge(const Pointer &batch, const Interval &batchRange, const Interval &mergeRange, const Interval &pmaInputRange, const Interval& pmaOutputRange) {
        IndexType outputBlockNumber = 0;
        PmaEntryType insertionBlock = PmaEntryType();
        const IndexType mergeBlock = pmaInputRange.left;
        insertionBlock.allocateBlock(allocator);
        IndexType batchIndex = batchRange.left;
        BlockIndexType readIndex = static_cast<BlockIndexType>(mergeRange.left);
        KeyType lastWrittenKey = NULL_KEY;
        while (batchIndex < batchRange.right || static_cast<IndexType>(readIndex) < mergeRange.right) {
            if (insertionBlock.size == BlockSize) {
                //Block is full, write it to output and get a new one
                PmaEntryType newBlock{};
                newBlock.allocateBlock(allocator);
                writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
                ++outputBlockNumber;
                insertionBlock = newBlock;
            }
            if (batchIndex < batchRange.right && static_cast<IndexType>(readIndex) < mergeRange.right) {
                //Insert the smaller of the two possible elements into the block. Prefer the element from mergeBlock on equality.
                if (comp(KeyOf::keyOfValue(pma[mergeBlock].get(readIndex)), KeyOf::keyOfValue(batch[batchIndex]))) {
                    //Insert from the mergeBlock
                    insertionBlock.insertWithIndex(pma[mergeBlock].get(readIndex), insertionBlock.size);
                    lastWrittenKey = KeyOf::keyOfValue(pma[mergeBlock].get(readIndex));
                    ++readIndex;
                } else if (KeyOf::keyOfValue(pma[mergeBlock].get(readIndex)) == KeyOf::keyOfValue(batch[batchIndex])) {
                    //Insert from the merge block, ignore the current batch element (duplicate key)
                    insertionBlock.insertWithIndex(pma[mergeBlock].get(readIndex), insertionBlock.size);
                    lastWrittenKey = KeyOf::keyOfValue(pma[mergeBlock].get(readIndex));
                    ++readIndex;
                    ++batchIndex;
                } else {
                    //Insert from the batch
                    if (KeyOf::keyOfValue(batch[batchIndex]) != lastWrittenKey) {
                        insertionBlock.insertWithIndex(batch[batchIndex], insertionBlock.size);
                        lastWrittenKey = KeyOf::keyOfValue(batch[batchIndex]);
                    }
                    ++batchIndex;
                }
            } else if (batchIndex < batchRange.right && static_cast<IndexType>(readIndex) >= mergeRange.right) {
                while (KeyOf::keyOfValue(batch[batchIndex]) == lastWrittenKey) {
                    ++batchIndex;//Skip duplicates at the start of the batch range.
                }
                //Merge as many elements from the remaining part of the batch as possible
                IndexType numberOfInsertions = std::min(static_cast<IndexType>(BlockSize - insertionBlock.size), batchRange.right - batchIndex);
                insertionBlock.merge(batch + batchIndex, static_cast<BlockSizeType>(numberOfInsertions));
                batchIndex += numberOfInsertions;
            } else if (batchIndex >= batchRange.right && static_cast<IndexType>(readIndex) < mergeRange.right) {
                //Merge the remaining part of the block
                BlockSizeType numberOfInsertions = std::min(static_cast<BlockSizeType>(BlockSize - insertionBlock.size), static_cast<BlockSizeType>(mergeRange.right - readIndex));
                insertionBlock.merge(pma[mergeBlock].block->begin() + readIndex, numberOfInsertions);
                readIndex += numberOfInsertions;
            } else {
                AssertMsg(false, "Should not happen!");
            }
        }
        if (insertionBlock.size > 0) {
            writeBlock(insertionBlock, pmaOutputRange.left, outputBlockNumber);
            ++outputBlockNumber;
        } else {
            insertionBlock.deleteBlock(allocator);
        }
        AssertMsg(batchIndex == batchRange.right, "Entire batch was inserted.");
        AssertMsg(readIndex == mergeRange.right, "Entire merge block section was inserted.");
        return outputBlockNumber;
    }

    /**
     * Uses exponentialSearch search to find the last block in the range [lower, upper)
     * that has head <= e.
     * Returns the index of the desired block
     */
    IndexType exponentialSearch(KeyType e, IndexType lower, IndexType upper) {
        IndexType start = lower;
        IndexType end = lower;
        while(end < upper && empty(end)) ++end;
        SizeType delta = 1;
        //Use exponential search to find first entry that has head > e...
        while (end < upper && (comp(head(end), e) || head(end) == e)) {
            start = end;
            end = findNext(end + delta, upper);
            delta *= 2;
        }
        return binarySearch(e, start, std::min(upper, end));//Then use binary search in the last interval
    }

    /**
     * Uses binary search to find the last block in the range [lower, upper)
     * that has head <= e.
     * Returns the index of the desired block
     */
    IndexType binarySearch(KeyType e, IndexType lower, IndexType upper) {
        IndexType left = lower, right = upper - 1;//inclusive boundaries
        IndexType candidate = -1;
        while (right - left > BinarySearchCutoff) {
            const IndexType mid = (left + right) / 2;
            IndexType prev = mid;
            while (prev >= left && empty(prev)) --prev;
            if (prev < left) {
                left = mid + 1;//No non-empty block in the left half
            } else {
                if (head(prev) == e) {
                    AssertMsg(prev == linearSearch(e, lower, upper), "Binary search was wrong");
                    return prev;//Found the block
                } else if (comp(e, head(prev))) {
                    //The current head is larger than the element, go left
                    right = prev - 1;
                } else {
                    //Go right
                    left = mid + 1;
                    //If it turns out that there  is no more element to the right, take prev as it is the first larger one.
                    candidate = prev;
                }
            }
        }
        auto result = linearSearch(e, left, upper);
        if (empty(result) || (comp(e, head(result)) && result != lower)) {
            //The result we got was wrong, there was no more element to the left, use candidate
            AssertMsg(candidate != -1 && !empty(candidate), "Candidate must be set");
            AssertMsg(candidate == linearSearch(e, lower, upper), "Binary search was wrong");
            return candidate;
        }
        AssertMsg(result == linearSearch(e, lower, upper), "Binary search was wrong");
        return result;
    }

    /**
     * Uses linear search to find the last block in the range [lower, upper)
     * that has head <= e.
     * Returns the index of the desired block.
     */
    IndexType linearSearch(KeyType e, IndexType lower, IndexType upper) {
        IndexType firstLarger = findNext(lower, upper);
        while (firstLarger < upper && !empty(firstLarger) && (comp(head(firstLarger), e) || e == head(firstLarger))) {
            //Go to next block.
            firstLarger = findNext(firstLarger, upper);
        }
        return findPrevious(firstLarger, lower);
    }

    /**
     * Finds the first index <= startIndex such that the batch element at that index is >= blockHead.
     */
    IndexType findFirstBatchElement(Pointer batch, IndexType startIndex, KeyType blockHead) {
        IndexType result = startIndex;
        while (result >= 0 && (comp(blockHead, KeyOf::keyOfValue(batch[result])) || KeyOf::keyOfValue(batch[result]) == blockHead)) {
            --result;
        }
        return result + 1;
    }

    /**
     * Finds the next larger entry that has a non-empty entry and is < upperLimit.
     */
    IndexType findNext(IndexType start, IndexType upperLimit) const {
        IndexType current = start + 1;
        while(current < upperLimit && empty(current)) ++current;
        return current;
    }

    /**
     * Finds the next smaller entry that has a non-empty entry and is >= lowerLimit.
     */
    IndexType findPrevious(IndexType start, IndexType lowerLimit) const {
        IndexType current = start - 1;
        while(current > lowerLimit && empty(current)) --current;
        return current;
    }

    /**
     * Finds the index of the block that the given element should be contained in.
     * Always returns an index of a non-empty position.
     */
    IndexType findBlockIndex(KeyType e) {
        const IndexType containingBlock = findContainingBlock(e);
        if (containingBlock == -1) {
            //Element is smaller than all others, find the first non-empty block for it
            IndexType firstBlock = 0;
            while (firstBlock < size && empty(firstBlock)) ++firstBlock;
            return firstBlock;
        }
        return containingBlock;
    }

    /**
     * Finds the index of the block that has to contain the element, if it exists.
     */
    IndexType findContainingBlock(KeyType e) {
        IndexType left = 0, right = size - 1;//inclusive boundaries
        IndexType candidate = -1;
        while (right - left > BinarySearchCutoff) {
            const IndexType mid = (left + right) / 2;
            IndexType prev = mid;
            while (prev >= left && empty(prev)) --prev;
            if (prev < left) {
                left = mid + 1;
            } else {
                if (comp(e, head(prev))) {
                    right = prev - 1;
                } else if (comp(head(prev), e)) {
                    candidate = prev;//Prev is still possible. If we do not find anything else, take it.
                    left = prev + 1;
                } else {//head(prev) == e
                    return prev;//Found the block
                }
            }
        }
        //Linear search the result:
        IndexType result = linearSearch(e, left, right + 1);
        if (empty(result) || comp(e, head(result))) {
            if (candidate != -1) {
                //The result we got was wrong, there was no more element to the left, use candidate
                AssertMsg(candidate != -1 && !empty(candidate), "Candidate must be set");
                return candidate;
            } else {
                //Element is smaller than all others.
                return -1;
            }
        }
        return result;
    }

    void distributeWorkInMergedTasks(Pointer batch, SizeType batchSize, UnsignedIndexType numberOfThreads) {
        for (UnsignedIndexType thread = 1; thread < numberOfThreads; ++thread) {
            if (tasks[thread].type == TaskType::INSERTION && tasks[thread].mergedTasks > 0) {
                //The current task is the last task of a row of merged tasks.
                //Make this task do the work of all the remaining blocks,
                //and all the previous tasks to the work of the one block for which they were merged.
                const IndexType mergeBlock = tasks[thread].blockForBatchSplitter;
                const Interval totalBatchRange(tasks[thread].firstBatchElement, tasks[thread + 1].firstBatchElement);
                const Interval pmaInputRange(tasks[thread].blockForBatchSplitter, tasks[thread + 1].blockForBatchSplitter);
                const IndexType firstNormalBlock = findNext(mergeBlock, pmaInputRange.right);
                /*
                std::cout << "Last task " << thread << " has " << tasks[thread].mergedTasks << " merged tasks." << std::endl;
                std::cout << "Last task " << thread << " has batch range " << totalBatchRange << std::endl;
                std::cout << "Last task " << thread << " has pma Input range " << pmaInputRange << std::endl;
                std::cout << "mergeBlock: " << mergeBlock << " with head" << head(mergeBlock) << std::endl;
                std::cout << "firstNormalBlock: " << firstNormalBlock << " with head" << head(firstNormalBlock) << std::endl;
                */
                IndexType lastTaskBatchStart;
                if (empty(firstNormalBlock)) {
                    //The mergeBlock is the last non-empty block overall, the last task does not get any work
                    lastTaskBatchStart = totalBatchRange.right;
                } else {
                    //1. Let this task only do batch elements that do not concern the mergeBlock
                    //the lastTaskBatchStart should be the first batch element that is >= head(firstNormalBlock)
                    lastTaskBatchStart = std::lower_bound(batch, batch + batchSize, head(firstNormalBlock), [](const ValueType &val, const KeyType &nextHead) -> bool {
                        return KeyOf::keyOfValue(val) < nextHead;
                    }) - batch;
                    lastTaskBatchStart = std::min(lastTaskBatchStart, totalBatchRange.right);
                }
                AssertMsg(lastTaskBatchStart >= totalBatchRange.left && lastTaskBatchStart <= totalBatchRange.right, "Illegal new batch start for last task");
                if (lastTaskBatchStart != totalBatchRange.left) {
                    //There is actual work for the merged tasks, adjust the last task appropriately
                    tasks[thread].firstBatchElement = lastTaskBatchStart;
                    tasks[thread].blockForBatchSplitter = firstNormalBlock;
                }
                //AssertMsg(numberOfElements() == 0 || !empty(mergeBlock), "Merge block is empty");

                //2. Distribute the remaining part of the batch to the mergedTasks many tasks in front of this one, give them all the merged block
                const Interval mergeBatchRange(totalBatchRange.left, lastTaskBatchStart);
                const IndexType elementsPerThread = mergeBatchRange.length() / tasks[thread].mergedTasks + 1;
                for (UnsignedIndexType mergeThread = tasks[thread].mergedTasks; mergeThread > 0; --mergeThread) {
                    //Make thread nr. thread - mergeThread do a part of the work
                    const IndexType mergeTaskBatchStart = std::min(mergeBatchRange.left + (tasks[thread].mergedTasks - mergeThread) * elementsPerThread, static_cast<UnsignedIndexType>(mergeBatchRange.right));
                    tasks[thread - mergeThread].firstBatchElement = mergeTaskBatchStart;
                    tasks[thread - mergeThread].blockForBatchSplitter = mergeBlock;
                    tasks[thread - mergeThread].mergedTasks = tasks[thread].mergedTasks;
                    tasks[thread - mergeThread].mergedTaskIndex = tasks[thread].mergedTasks - mergeThread;
                    tasks[thread - mergeThread].requiredWidth = maxRedistributionWidth(blockSize(mergeBlock), elementsPerThread);
                }
            }
        }
    }

    void startTaskInPlace(UnsignedIndexType taskIndex, Pointer batch) {
        AssertMsg(tasks[taskIndex].type == TaskType::INSERTION, "In place tasks must be insertion only.");
        Interval batchRange(tasks[taskIndex].firstBatchElement, tasks[taskIndex + 1].firstBatchElement);
        if (batchRange.length() == 0) return;//Nothing to do
        Interval pmaInputRange(tasks[taskIndex].blockForBatchSplitter, tasks[taskIndex + 1].blockForBatchSplitter);
        if constexpr (Debug) std::cout << "Starting in-place-task " << taskIndex << " in interval " << pmaInputRange << std::endl;
        insertBatchRange(batch, batchRange, pmaInputRange);
    }

    void startTaskOutOfPlace(UnsignedIndexType taskIndex, Pointer batch, SizeType batchSize) {
        Interval batchRange(tasks[taskIndex].firstBatchElement, tasks[taskIndex + 1].firstBatchElement);
        Interval pmaInputRange(tasks[taskIndex].blockForBatchSplitter, tasks[taskIndex + 1].blockForBatchSplitter);
        Interval pmaOutputRange(tasks[taskIndex].startIndex, tasks[taskIndex + 1].startIndex);
        if (taskIndex == 0) {
            //Copy the blocks in front of the pmaInputRange
            for (IndexType blockIndex = 0; blockIndex < pmaInputRange.left; ++blockIndex) {
                if (!empty(blockIndex)) {
                    swapPma[blockIndex] = pma[blockIndex];
                    pma[blockIndex] = PmaEntryType();
                }
            }
        }
        if (tasks[taskIndex].type == TaskType::INSERTION) {
            if (batchRange.length() == 0) {
                tasks[taskIndex].numberOfBlocksWritten = 0;
                //Nothing to do, just copy the blocks so that they are not lost.
                for (IndexType blockIndex = pmaInputRange.left; blockIndex < pmaInputRange.right; ++blockIndex) {
                    if (!empty(blockIndex)) {
                        writeBlock(blockIndex, pmaOutputRange.left, tasks[taskIndex].numberOfBlocksWritten);
                        ++tasks[taskIndex].numberOfBlocksWritten;
                    }
                }
                return;
            }
            if constexpr (Debug) std::cout << "Starting task " << taskIndex << ", length " << pmaInputRange.length() << ", --> " << pmaOutputRange.length() << std::endl;
            //std::cout << "Execute insertion task with batch range " << batchRange << ", pmaInput" << pmaInputRange << ", pmaOutput " << pmaOutputRange << std::endl;
            tasks[taskIndex].numberOfBlocksWritten = insertBatchRange(batch, batchRange, pmaInputRange, pmaOutputRange);
        } else {
            AssertMsg(UseInverseMerge, "illegal task type.");
            AssertMsg(tasks[taskIndex].type == TaskType::INVERSE_MERGE, "Wrong task type.");
            if (batchRange.length() == 0) {
                tasks[taskIndex].numberOfBlocksWritten = 0;
                return;//Nothing to do
            }
            //std::cout << "   Start inverse merge task with index " << tasks[taskIndex].mergedTaskIndex << " / " << tasks[taskIndex].mergedTasks << std::endl;
            const IndexType mergeBlock = pmaInputRange.left;
            //std::cout << "   Merge block is " << mergeBlock << std::endl;
            Interval mergeRange(0, pma[mergeBlock].size);
            //Merge elements that are >= the batch start, of from the start of the block for the first subtask
            if (pma[mergeBlock].size != 0 && tasks[taskIndex].mergedTaskIndex != 0) {
                AssertMsg(batchRange.left != batchSize, "There is a valid batch element to be inserted.");
                mergeRange.left = pma[mergeBlock].findIndex(KeyOf::keyOfValue(batch[batchRange.left]));
            }
            //Merge all elements that are smaller than the next subtask's head, or to the end of the block for the last subtask
            if (pma[mergeBlock].size != 0 && tasks[taskIndex].mergedTaskIndex + 1 != tasks[taskIndex].mergedTasks) {
                if (batchRange.right != batchSize) {
                    mergeRange.right = pma[mergeBlock].findIndex(KeyOf::keyOfValue(batch[batchRange.right]));
                }
                //Otherwise: Keep mergeRange.right = merge block size; all the following tasks have batchRange.length() == 0,
                //So there is no work for them.
            }
            tasks[taskIndex].numberOfBlocksWritten = inverseMerge(batch, batchRange, mergeRange, pmaInputRange, pmaOutputRange);
        }
    }

    void postprocessInverseMergeTasks(UnsignedIndexType numberOfThreads) {
        //Postprocessing: Merge blocks that are too small and remove the blocks that were merged inversely.
        PmaEntryType carryBlock{};
        for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread) {
            if (carryBlock.block != nullptr && tasks[thread].numberOfBlocksWritten > 0) {
                //Merge the carry block into the task's first block
                AssertMsg(carryBlock.size > 0, "Merging empty carry block.");
                if (swapPma[tasks[thread].startIndex].size + carryBlock.size < BlockSize) {
                    //All elements fit into the block, merge directly
                    swapPma[tasks[thread].startIndex].mergeSmaller(carryBlock);
                    carryBlock.deleteBlock(allocator);
                } else {
                    //There are more than BlockSize total elements, create two blocks that are at least half full
                    carryBlock.steal(swapPma[tasks[thread].startIndex]);
                    //Write the carry block to the end of the *previous task*:
                    //If it came from there, there is always space. Otherwise, the task is empty but was given width > 0 before.
                    const IndexType previousTaskEnd = tasks[thread - 1].startIndex + tasks[thread - 1].numberOfBlocksWritten;
                    swapPma[previousTaskEnd] = carryBlock;
                    ++tasks[thread - 1].numberOfBlocksWritten;
                }
                carryBlock = PmaEntryType();
            }
            if (tasks[thread].type == TaskType::INVERSE_MERGE && tasks[thread].numberOfBlocksWritten > 0) {
                //Check if the last block has sufficient size
                const IndexType lastWrittenBlock = tasks[thread].startIndex + tasks[thread].numberOfBlocksWritten - 1;
                AssertMsg(swapPma[lastWrittenBlock].size > 0, "Last block has invalid size");
                if (swapPma[lastWrittenBlock].size < MinBlockSize) {
                    //The last block this task has written is too small. Remove it and keep for later.
                    AssertMsg(carryBlock.block == nullptr, "Carry block already used!");
                    carryBlock = swapPma[lastWrittenBlock];
                    swapPma[lastWrittenBlock] = PmaEntryType();
                    --tasks[thread].numberOfBlocksWritten;//One block was removed from this task.
                }
            }
            if (thread > 0 && tasks[thread].type == TaskType::INSERTION && tasks[thread - 1].type == TaskType::INVERSE_MERGE) {
                //Task thread is the last task of a merge group, we can safely delete the mergeBlock here.
                const IndexType mergeBlock = tasks[thread - 1].blockForBatchSplitter;
                if (!empty(mergeBlock)) {//Merge block is only empty if the sds is empty.
                    pma[mergeBlock].deleteBlock(allocator);
                    pma[mergeBlock] = PmaEntryType();//Null the entry
                }
            }
        }
        if (carryBlock.block != nullptr) {
            //There was no task to the right that the carry block could have been merged into, merge it to the
            //end of the task it originates from.
            for (UnsignedIndexType thread = numberOfThreads; thread-- != 0;) {//! This is a loop from numberOfThreads-1 downto 0
                if (tasks[thread].numberOfBlocksWritten > 0) {
                    //Merge the carry block with the last block of this task
                    const IndexType lastWrittenBlock = tasks[thread].startIndex + tasks[thread].numberOfBlocksWritten - 1;
                    if (swapPma[lastWrittenBlock].size + carryBlock.size < BlockSize) {
                        //All elements fit into the block, merge directly
                        //swapPma[lastWrittenBlock] is a block with elements smaller than those of carryBlock (which originates from a place further right)
                        //Therefore, swap the roles of swapPma[lastWrittenBlock] and carryBlock and use the usual functionality
                        //to merge the old swapPma[lastWrittenBlock] into carryBlock
                        carryBlock.mergeSmaller(swapPma[lastWrittenBlock]);
                        swapPma[lastWrittenBlock].deleteBlock(allocator);
                        swapPma[lastWrittenBlock] = carryBlock;
                        carryBlock = PmaEntryType();
                    } else {
                        //There are more than BlockSize total elements, create two blocks that are at least half full
                        //Steal elements from swapPma[lastWrittenBlock] into carryBlock
                        carryBlock.stealFromSmaller(swapPma[lastWrittenBlock]);
                        IndexType writeIndex;
                        if (tasks[thread].numberOfBlocksWritten < tasks[thread].requiredWidth) {
                            //This task still has space, write the block here
                            writeIndex = tasks[thread].startIndex + tasks[thread].numberOfBlocksWritten;
                            ++tasks[thread].numberOfBlocksWritten;
                        } else {
                            //If task thread is still full, the carry block must have originated from a task to the right.
                            //Therefore, there is a task thread + 1. It has requiredWidth > 0, so one block can be written safely.
                            AssertMsg(thread + 1 < numberOfThreads, "There is no further task.");
                            AssertMsg(tasks[thread + 1].numberOfBlocksWritten == 0, "Next task has also written a block");
                            writeIndex = tasks[thread + 1].startIndex;
                            ++tasks[thread + 1].numberOfBlocksWritten;
                        }
                        swapPma[writeIndex] = carryBlock;
                        carryBlock = PmaEntryType();
                    }
                    break;
                }
            }
        }
        AssertMsg(carryBlock.block == nullptr, "Carry was written again.");
    }

    void startRewritingTask(UnsignedIndexType taskIndex) {
        if (taskIndex == 0) {
            //Copy the blocks in front of the pmaInputRange of this task
            for (IndexType blockIndex = 0; blockIndex < tasks[taskIndex].blockForBatchSplitter; ++blockIndex) {
                if (!swapPma[blockIndex].isEmpty()) {
                    pma[blockIndex] = swapPma[blockIndex];
                    swapPma[blockIndex] = PmaEntryType();//Remove the old block
                }
            }
        }
        //The range that must be rewritten from swapPma to pma
        Interval pmaOutputRange(tasks[taskIndex].startIndex, tasks[taskIndex + 1].startIndex);
        rewrite(pmaOutputRange, tasks[taskIndex].numberOfBlocksWritten);
    }

    /**
     * Uniformly redistributes the numberOfBlocks blocks within range
     * while moving them from swapPma to pma.
     */
    void rewrite(const Interval &range, SizeType numberOfBlocks) {
        for (IndexType currentBlock = 0; currentBlock < numberOfBlocks; ++currentBlock) {
            const IndexType readIndex = range.left + currentBlock;
            AssertMsg(swapPma[readIndex].block != nullptr, "Not enough blocks to rewrite.");
            const IndexType writeIndex = getUniformWriteIndex(range.left, currentBlock, range.length(), numberOfBlocks);
            AssertMsg(writeIndex < size, "Writing behind active end");
            AssertMsg(writeIndex >= range.left && writeIndex < range.right, "Writing outside of the range");
            AssertMsg(empty(writeIndex), "Writing to an occupied entry");
            pma[writeIndex] = swapPma[readIndex];
            swapPma[readIndex] = PmaEntryType();//Remove the old block
        }
    }

    IndexType getUniformWriteIndex(IndexType outputStart, IndexType blockNumber, SizeType outputLength, SizeType numberOfBlocks) {
        AssertMsg(blockNumber < numberOfBlocks, "Writing too many blocks");
        return outputStart + (blockNumber * outputLength) / numberOfBlocks;
    }

    void resizeMainPma() {
         if (swapCapacity != capacity) {
             pma = std::make_unique<PmaEntryType[]>(swapCapacity);
             if constexpr (Debug) std::cout << "Resized pma to " << swapCapacity << std::endl;
             capacity = swapCapacity;
         }
    }

    /**
     * Increases the size to the given new value.
     * If necessary, increases capacity and reallocates the swap data structures to the larger capacity.
     */
    void increaseSize(IndexType newSize) {
        if (newSize >= swapCapacity) {
            //Resize necessary.
            while (swapCapacity <= newSize) {
                swapCapacity *= 2;
            }
            swapPma = std::make_unique<PmaEntryType[]>(swapCapacity);
        }
        size = newSize;
    }

    SizeType maxRedistributionWidth(SizeType numberOfElements, SizeType newElements) {
        //Assume only that in the end, all blocks will be at least half full.
        AssertMsg(numberOfElements >= 0 && newElements >= 0, "Negative numbers of elements.");
        const SizeType newNumberOfElements = numberOfElements + newElements;
        const SizeType newNumberOfBlocks = newNumberOfElements == 0 ? 0 : newNumberOfElements / MinBlockSize + 1;
        if constexpr (Debug) std::cout << "For previous number of elements " << numberOfElements << " and " << newElements << " new elements, need " << newNumberOfBlocks << " blocks." << std::endl;
        return newNumberOfBlocks;
    }

    /**
     * Tries to write the given block to targetIndex.
     * If targetIndex is occupied, tries to make space by shifting elements to the right.
     * If all slots to the right are occupied, shifts elements to the left of targetIndex further left
     * and inserts at an index to the left of targetIndex, such that all elements that previously
     * were at indices >= targetIndex are now at indices > the one written to.
     * Returns the index that the block was actually written to.
     */
    IndexType writeBlockInPlace(const PmaEntryType &newBlock, IndexType targetIndex, const Interval &pmaRange) {
        AssertMsg(pmaRange.right <= size, "Working behind size");
        AssertMsg(targetIndex >= pmaRange.left && targetIndex <= pmaRange.right, "Invalid target index.");
        if (targetIndex == pmaRange.right) {
            //Trying to add a new *last* element in the range. Shift the suffix in pmaRange left.
            IndexType shift = targetIndex - 1;
            PmaEntryType carry = newBlock;
            while (shift >= pmaRange.left && carry.block != nullptr) {
                std::swap(pma[shift], carry);
                --shift;
            }
            AssertMsg(carry.block == nullptr, "Not enough space.");
            sizeManager.adjustBlockSize(targetIndex - 1, 0, newBlock.size);
            sizeManager.shiftLeft(shift + 1, targetIndex - 2);
            return targetIndex - 1;
        }
        IndexType insertionIndex = targetIndex;
        if (!empty(targetIndex)) {
            //Make space for the target index by shifting elements to the right
            IndexType shift = targetIndex;
            PmaEntryType carry = newBlock;//Swap the new pma entry into the array at targetIndex
            while (shift < pmaRange.right && carry.block != nullptr) {
                std::swap(pma[shift], carry);
                ++shift;
            }
            if (carry.block == nullptr) {
                //The right shift worked
                sizeManager.adjustBlockSize(targetIndex, 0, newBlock.size);
                sizeManager.shiftRight(targetIndex + 1, shift - 1);//targetIndex + 1 is the first index that was swapped to, shift -1 the last one
            } else {
                AssertMsg(shift == pmaRange.right, "Shift behaved unexpectedly.");
                //We shifted all elements to the right, but there is no free slot in the pmaRange. Shift everything back to the left
                shift = pmaRange.right - 1;
                while (shift >= pmaRange.left && carry.block != nullptr) {
                    std::swap(pma[shift], carry);
                    --shift;
                }
                insertionIndex = targetIndex - 1;//block was shifted left by one slot, too.
                sizeManager.adjustBlockSize(insertionIndex, 0, newBlock.size);
                sizeManager.shiftLeft(shift + 1, insertionIndex - 1);//shift + 1 is the last index that an old block was shifted to, insertionIndex - 1 the first one, that got a new value
            }
        } else {
            sizeManager.adjustBlockSize(targetIndex, 0, newBlock.size);
            pma[targetIndex] = newBlock;
        }
        AssertMsg(head(insertionIndex) == KeyOf::keyOfValue(pma[insertionIndex].get(0)), "Wrong head");
        return insertionIndex;
    }

    void writeBlock(IndexType blockIndex, IndexType outputStart, IndexType blockNumber) {
        AssertMsg(!pma[blockIndex].isEmpty(), "Writing a null block.");
        const IndexType writeIndex = getDenseWriteIndex(outputStart, blockNumber);
        AssertMsg(writeIndex < swapCapacity, "Writing to an invalid index");
        AssertMsg(writeIndex < size, "Writing to an invalid index");
        AssertMsg(swapPma[writeIndex].block == nullptr, "Overwriting a block");
        swapPma[writeIndex] = pma[blockIndex];
        pma[blockIndex] = PmaEntryType();
    }

    void writeBlock(const PmaEntryType &block, IndexType outputStart, IndexType blockNumber) {
        AssertMsg(!block.isEmpty(), "Writing a null block.");
        const IndexType writeIndex = getDenseWriteIndex(outputStart, blockNumber);
        AssertMsg(writeIndex < swapCapacity, "Writing to an invalid index");
        AssertMsg(writeIndex < size, "Writing to an invalid index");
        AssertMsg(swapPma[writeIndex].block == nullptr, "Overwriting a block");
        swapPma[writeIndex] = block;
    }

    IndexType getDenseWriteIndex(IndexType outputStart, IndexType blockNumber) const {
        return outputStart + blockNumber;
    }

    bool empty(IndexType index) const {
        AssertMsg((head(index) == NULL_KEY) == pma[index].isEmpty(), "Cell " << index << " has invalid state.");
        AssertMsg((head(index) == NULL_KEY) == (blockSize(index) == 0), "Cell " << index << " has invalid state.");
        return head(index) == NULL_KEY;
    }

public:
    iterator begin() {
        IndexType first = 0;
        while (empty(first) && first != size) ++first;
        return iterator(this, first, 0);
    }

    iterator end() {
        return iterator(this, size, 0);
    }

    [[nodiscard]] KeyType& head(IndexType index) {
        return pma[index].head;
    }

    [[nodiscard]] KeyType head(IndexType index) const {
        return pma[index].head;
    }

    [[nodiscard]] BlockSizeType& blockSize(IndexType index) {
        return pma[index].size;
    }

    [[nodiscard]] BlockSizeType blockSize(IndexType index) const {
        return pma[index].size;
    }

public:
    // For debugging only.
    void printStats() {
        std::cout << "Batched sparse array with"
            << " capacity " << capacity
            << ", number of elements " << numberOfElements()
            << ", active region [" << 0 << ", " << size << ")"
            << ", is valid? " << isValid()
            << ", number of blocks " << numberOfBlocks()
            << ", density " << (static_cast<double>(numberOfBlocks()) / static_cast<double>(size))
            << std::endl;
    }

    SizeType numberOfElements() const {
        IndexType n = 0;
        for (IndexType i = 0; i < size; ++i) {
            if (!empty(i)) {
                n += blockSize(i);
            }
        }
        return n;
    }

    SizeType numberOfBlocks() const {
        IndexType nBlocks = 0;
        for (IndexType i = 0; i < size; ++i) {
            if (!empty(i)) {
                ++nBlocks;
            }
        }
        return nBlocks;
    }

    void printContent() const {
        for (IndexType i = 0; i < size; ++i) {
            if (!empty(i)) {
                std::cout << "Block " << i << ": " << *(pma[i].block) << std::endl;
            }
        }
        std::cout << "ActiveEnd = " << size << std::endl;
    }

    void printBlockStructure() const {
        for (IndexType i = 0; i < size; ++i) {
            if (!empty(i)) {
                std::cout << "X";
            } else {
                std::cout << "_";
            }
        }
        std::cout << std::endl;
    }

    void printHeads() const {
        for (IndexType i = 0; i < size; ++i) {
            std::cout << head(i) << " ";
        }
        std::cout << std::endl;
    }

    void print() {
        printStats();
        printContent();
        isValid() ? std::cout << "Valid." << std::endl : std::cout << " ERROR: INVALID structure, not sorted correctly." << std::endl;
    }

    bool __attribute__ ((noinline)) isValid() const {
        KeyType currentHead = NULL_KEY;
        KeyType currentElement = NULL_KEY;
        for (IndexType i = 0; i < capacity; ++i) {
            if (!empty(i)) {
                AssertMsg(pma[i].size <= BlockSize, "Block too large.");
                AssertMsg(pma[i].size >= MinBlockSize, "Block too small.");
                if (pma[i].size > BlockSize || pma[i].size < MinBlockSize) return false;
                if (comp(head(i), currentHead) || head(i) == currentHead) {
                    std::cout << "Head " << head(i) << " at " << i << " larger than previous " << currentHead << std::endl;
                    AssertMsg(false, "Not sorted correctly");
                    return false;//Not ordered or duplicate
                }
                currentHead = head(i);
                if (head(i) != KeyOf::keyOfValue(pma[i].get(0))) {
                    AssertMsg(false, "Wrong head");
                    std::cout << "Head " << head(i) << " != block start " << KeyOf::keyOfValue(pma[i].get(0)) << std::endl;
                    return false;
                }
                for (BlockIndexType j = 0; j < blockSize(i); ++j) {
                    KeyType element = KeyOf::keyOfValue(pma[i].get(j));
                    if (comp(element, currentElement) || element == currentElement) {
                        std::cout << "Block " << i << " not sorted correctly at element " << element << ", previous is " << currentElement << std::endl;
                        AssertMsg(false, "Not sorted correctly.");
                        return false;
                    }
                    currentElement = element;
                }
            } else {
                AssertMsg(pma[i].isEmpty() && blockSize(i) == 0, "Wrong pma values");
                if (!pma[i].isEmpty() || blockSize(i) != 0) return false;
            }
        }
        return true;
    }

    bool __attribute__ ((noinline)) validateSwapPma() const {
        KeyType currentHead = NULL_KEY;
        KeyType currentElement = NULL_KEY;
        for (IndexType i = 0; i < capacity; ++i) {
            if (swapPma[i].block != nullptr) {
                if (swapPma[i].head <= currentHead) {
                    AssertMsg(false, "not ordered");
                    std::cout << "Head " << swapPma[i].head << " at " << i << " larger than previous " << currentHead << std::endl;
                    return false; // Not ordered or duplicate
                }
                currentHead = swapPma[i].head;
                if (swapPma[i].head != KeyOf::keyOfValue(swapPma[i].get(0))) {
                    AssertMsg(false, "Wrong head");
                    return false;
                }
                for (BlockIndexType j = 0; j < swapPma[i].size; ++j) {
                    KeyType element = KeyOf::keyOfValue(swapPma[i].get(j));
                    if (element <= currentElement) {
                        AssertMsg(false, "not sorted correctly.");
                        return false;
                    }
                    currentElement = element;
                }
            } else {
                AssertMsg(swapPma[i].block == nullptr && swapPma[i].size == 0, "Wrong pma values");
                if (swapPma[i].block != nullptr || swapPma[i].size != 0) return false;
            }
        }
        return true;
    }

    void validateTaskDistribution(UnsignedIndexType numberOfThreads, SizeType batchSize, bool isInPlace) const {
        AssertMsg(tasks[0].firstBatchElement == 0, "Tasks 0 does not start at batch start");

        for (UnsignedIndexType thread = 0; thread <= numberOfThreads; ++thread) {
            if (!isInPlace && tasks[thread].type == TaskType::INVERSE_MERGE) {
                AssertMsg(tasks[thread].mergedTasks < numberOfThreads, "Invalid number of merged tasks");
                AssertMsg(tasks[thread].mergedTaskIndex < tasks[thread].mergedTasks, "Wrong merged task index");
            }
        }
        AssertMsg(tasks[numberOfThreads].firstBatchElement == batchSize, "Sentinel task does not start at batch start");
        AssertMsg(tasks[numberOfThreads].blockForBatchSplitter == size, "Sentinel task does not end at sds end");
    }

    bool batchSorted(Pointer batch, SizeType batchSize) const {
        KeyType last = NULL_KEY;
        for (IndexType i = 0; i < batchSize; ++i) {
            if (comp(KeyOf::keyOfValue(batch[i]), last))
                return false;
            last = KeyOf::keyOfValue(batch[i]);
        }
        return true;
    }

    const std::string type() {
        if constexpr (Profile || ProfileConcurrent) {
            return "GBPMA-b=" + std::to_string(BlockSize) + "-o=" + std::to_string(overallocationFactor) + "-profiled";
        } else {
            return "GBPMA-b=" + std::to_string(BlockSize) + "-o=" + std::to_string(overallocationFactor);
        }
    }

    BlockSizeType getBlockSize() const {
        return BlockSize;
    }
};
}
