#pragma once

#include <cstring>
#include <memory>

#include "batchedSparseArray.hpp"
#include "assert.hpp"
#include "typedefs.hpp"

namespace DataStructures {
/**
 * Block size manager that uses no auxiliary data structures and simply calculates the
 * number of elements with a linear reduction.
 */
template<typename SDS_TYPE>
class LinearBlockSizeManager {
    using SdsType = SDS_TYPE;

private:
    SdsType *sds;

public:
    LinearBlockSizeManager(SdsType *pointer) : sds(pointer) {}

    void addElement(IndexType blockIndex) {
        (void) blockIndex;
    }

    SizeType getNumberOfElements(IndexType left, IndexType right) const {
        SizeType sum = 0;
        for (IndexType current = left; current < right; ++current) sum += sds->blockSize(current);
        return sum;
    }

    void adjustBlockSize(IndexType blockIndex, BlockSizeType oldSize, BlockSizeType newSize) {
        (void) oldSize;//Might be used in other implementations
    }

    void shiftLeft(IndexType left, IndexType right) const {
        //Nothing to do in this variant.
        (void) left;
        (void) right;
    }
    void shiftRight(IndexType left, IndexType right) const {
        //Nothing to do in this variant
        (void) left;
        (void) right;
    }
    void recompute(UnsignedSizeType numberOfThreads) const {
        //Nothing to do in this variant
        (void) numberOfThreads;
    }
    void validate() const {}
};

/**
 * Block size manager that uses a hierarchy of aggregated size information to compute
 * the number of elements in a range more efficiently.
 *
 * Currently, uses only one level.
 */
template<typename SDS_TYPE>
class HierarchicalBlockSizeManager {
    //static constexpr SizeType AggregateWidth = 64;//Number of elements that is aggregated on each level.
    using SdsType = SDS_TYPE;
    using AggregateType = std::atomic<SizeType>;

private:
    SdsType *sds;
    std::unique_ptr<AggregateType[]> aggregates;
    SizeType size;

public:
    SizeType AggregateWidth = 64;

public:
    HierarchicalBlockSizeManager(SdsType *pointer) : sds(pointer) {
        size = aggregateIndex(sds->size, 0) + 1;
        aggregates = std::make_unique<AggregateType[]>(size);
    }

    SizeType getNumberOfElements(IndexType left, IndexType right) const {
        const IndexType aggregateStart = nextAggregate(left, 0);
        const IndexType aggregateEnd = aggregateIndex(right, 0);//exclusive end
        SizeType sum = 0;
        if (aggregateStart < aggregateEnd) {
            //Range to the left of the aggregated range
            for (IndexType current = left; current < getBlockIndex(aggregateStart, 0); ++current) {
                sum += sds->blockSize(current);
            }
            //Reduction over the aggregates
            for (IndexType current = aggregateStart; current < aggregateEnd; ++current) {
                sum += aggregates[current].load(std::memory_order_relaxed);//global read only access, no concurrent writes
            }
            //Range to the right of the aggregated range
            for (IndexType current = getBlockIndex(aggregateEnd, 0); current < right; ++current) {
                sum += sds->blockSize(current);
            }
        } else {
            //No aggregated block fully contained, calculate manually.
            for (IndexType current = left; current < right; ++current) sum += sds->blockSize(current);
        }
        return sum;
    }

    /**
     * Recomputes the entire auxiliary data after adjusting to a possibly changed sds array size.
     */
    void recompute(UnsignedSizeType numberOfThreads) {
        //Reallocate
        const SizeType newSize = aggregateIndex(sds->size, 0) + 1;
        if (newSize != size) {
            size = newSize;
            aggregates = std::make_unique<AggregateType[]>(size);
        }

        //Recompute the data
#pragma omp parallel for schedule(static) num_threads(numberOfThreads)
        for (IndexType i = 0; i < std::min(size, aggregateIndex(sds->size, 0) + 1); ++i) {
            SizeType aggregate = 0;
            for (IndexType current = getBlockIndex(i, 0); current < getBlockIndex(i + 1, 0); ++current) aggregate += sds->blockSize(current);
            aggregates[i].store(aggregate, std::memory_order_relaxed);//working on disjoint regions, write-only
        }
    }

    void addElement(IndexType blockIndex) {
        aggregates[aggregateIndex(blockIndex, 0)].fetch_add(1, std::memory_order_relaxed);
    }

    void adjustBlockSize(IndexType blockIndex, BlockSizeType oldSize, BlockSizeType newSize) {
        aggregates[aggregateIndex(blockIndex, 0)].fetch_add(static_cast<SizeType>(newSize) - static_cast<SizeType>(oldSize), std::memory_order_relaxed);
    }

    /**
     * Adjusts the data structure after blocks were shifted to the left in the range [left, right],
     * i.e., the values that previously were at [left + 1, right + 1] are now in [left, right].
     */
    void shiftLeft(IndexType left, IndexType right) {
        //Run through range. At each aggregation border, subtract the respective block size from the right one and add it to the left
        const IndexType aggregateStart = aggregateIndex(left, 0) + 1;//border that is strictly behind left
        const IndexType aggregateEnd = aggregateIndex(right + 1, 0) + 1;//border that is strictly behind right + 1: a border at right + 1 will be considered
        for (IndexType current = aggregateStart; current < aggregateEnd; ++current) {
            const IndexType shiftedBlock = getBlockIndex(current, 0) - 1;
            //The block that used to be at shift origin was shifted to the left. Add it there, subtract it from current.
            aggregates[current].fetch_sub(sds->blockSize(shiftedBlock), std::memory_order_relaxed);
            aggregates[current - 1].fetch_add(sds->blockSize(shiftedBlock), std::memory_order_relaxed);
        }
    }

    /**
     * Adjusts the data structure after blocks were shifted to the right in the range [left, right],
     * i.e., the values that previously were at [left - 1, right - 1] are now in [left, right].
     */
    void shiftRight(IndexType left, IndexType right) {
        const IndexType aggregateStart = nextAggregate(left, 0);//border that is at or behind left
        const IndexType aggregateEnd = aggregateIndex(right, 0) + 1;//border that is behind right as exclusive end
        for (IndexType current = aggregateStart; current < aggregateEnd; ++current) {
            const IndexType shiftedBlock = getBlockIndex(current, 0);
            //The block that used to be at shift origin was shifted to the right. Add it there, subtract it from current - 1.
            aggregates[current - 1].fetch_sub(sds->blockSize(shiftedBlock), std::memory_order_relaxed);
            aggregates[current].fetch_add(sds->blockSize(shiftedBlock), std::memory_order_relaxed);
        }
    }

private:
    /**
     * Computes the index of the aggregate value for the given block size and the given level.
     * Level 0 is the first level (aka, the lowest above the size values)
     */
    IndexType aggregateIndex(IndexType blockIndex, IndexType level) const {
        (void) level;//Currently, there is only one level.
        return blockIndex / AggregateWidth;
    }

    /**
     * Computes the smallest index of an aggregate that covers a range that is does not include anything in front of blockIndex.
     */
    IndexType nextAggregate(IndexType blockIndex, IndexType level) const {
        (void) level;//Not used yet
        if (blockIndex % AggregateWidth == 0) {
            //Starting at the start of an aggregation interval, entire interval is contained
            return aggregateIndex(blockIndex, 0);
        } else {
            return aggregateIndex(blockIndex, 0) + 1;//The interval of the aggregateIndex is not fully contained.
        }
    }

    /**
     * Computes the first block that is covered by the given aggregate.
     */
    IndexType getBlockIndex(IndexType aggregateIndex, IndexType level) const {
        (void) level;//Not used yet
        return aggregateIndex * AggregateWidth;
    }

public:
    void validate() {
        for (IndexType i = 0; i < std::min(size, aggregateIndex(sds->size, 0) + 1); ++i) {
            SizeType aggregate = 0;
            for (IndexType current = getBlockIndex(i, 0); current < getBlockIndex(i + 1, 0); ++current) aggregate += sds->blockSize(current);
            AssertMsg(aggregate == aggregates[i].load(std::memory_order_relaxed), "Aggregate " << i << " is wrong   .");
        }
    }
};
}
