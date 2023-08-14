#pragma once

#include <cstring>
#include <memory>
#include <limits>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <omp.h>

#include "batchedSparseArray.hpp"
#include "assert.hpp"
#include "typedefs.hpp"

namespace DataStructures {
class ConcurrentBucketMap {
    static constexpr SizeType NumberOfBucketsFactor = 10;
    using InternalMap = std::unordered_map<IndexType, bool>;
#ifndef NDEBUG
    static constexpr SizeType LevelParallelizationThresholdPerThread = 0;//For debugging, use parallel way even for less work.
#else
    static constexpr SizeType LevelParallelizationThresholdPerThread = 10;//TODO Evaluate.
#endif

private:
    struct InternalBucket {
    public:
        std::mutex mutex;
        InternalMap map;
    };

public:
    std::vector<InternalBucket> buckets;
    UnsignedIndexType numberOfThreads;

public:
    ConcurrentBucketMap(UnsignedSizeType numThreads) :
        buckets(NumberOfBucketsFactor * numThreads),
        numberOfThreads(numThreads){
    }

    /**
     * Inserts the new entry to the respective map or adjusts the mapped value.
     */
    void insert(const std::pair<const IndexType, bool> &newEntry) {
        const IndexType bucket = getBucket(newEntry.first);
        //std::cout << "      Inserting entry into bucket " << bucket << std::endl;
        buckets[bucket].mutex.lock();
        InternalMap::iterator it;
        bool inserted;
        std::tie(it, inserted) = buckets[bucket].map.insert(newEntry);
        if (!inserted && newEntry.second && !it->second) {
            //Entry already there, but it needs to be rebalanced.
            it->second = true;
        }
        buckets[bucket].mutex.unlock();
    }

    template<typename F>
    void executeInParallel(SizeType parallelizationThreshold, F f) const {
#pragma omp parallel for schedule(dynamic) num_threads(numberOfThreads) if(numberOfElements() > parallelizationThreshold)
        for (const auto &bucket : buckets) {
            for (const auto &entry : bucket.map) {
                f(entry);
            }
        }
    }

    SizeType numberOfElements() const {
        SizeType result = 0;
        //std::cout << "Bucket map bucket sizes are: ";
        for (auto &bucket : buckets) {
            //std::cout << bucket.map.size() << " ";
            result += bucket.map.size();
        }
        //std::cout << std::endl;
        //std::cout << "Bucket map total size is " << result << std::endl;
        return result;
    }

    /**
     * Clears all the buckets
     */
    void clear() {
        for (auto &bucket : buckets) {
            bucket.map.clear();
        }
    }

    IndexType getBucket(IndexType index) const {
        return std::hash<IndexType>{}(index) % buckets.size();
    }

};
}
