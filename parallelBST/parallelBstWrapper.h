#pragma once

#include "helpers.hpp"
#include "assert.hpp"
#include "profiler.hpp"

#include "bulk_operations_data_structure.h"
#include "join_data_structure.h"
#include "split_data_structure.h"

template <typename KEY, typename PROFILER>
class ParallelBST {
public:
    using ValueType = KEY;
    using Reference = KEY&;
    using ConstReference = KEY const&;
    using Profiler = PROFILER;

    using TreeType = TBtree_4_8<true>;
    using BatchInserterType = TBulkInserter<TreeType, TDataInserter, TParallelSplit<TreeType>, TParallelPairwiseJoin<TreeType>>;

public:
    Profiler profiler;
    static constexpr bool Profile = std::is_same_v<PROFILER, Helpers::Profiler::SequentialProfiler>;

private:
    TreeType tree;
    uint32_t numberOfThreads;
    BatchInserterType bulkInserter;
    bool stillEmpty;

public:
    ParallelBST(uint32_t nThreads) :
        numberOfThreads(nThreads),
        bulkInserter(numberOfThreads),
        stillEmpty(true) {
        if (numberOfThreads == 1) {
            std::cout << "WARNING: The Parallel BST always uses at least two threads." << std::endl;
        }
    }

    bool contains(ConstReference e) {
        //TODO implement
        return false;
    }

    void insertPrefillBatch(std::vector<ValueType> batch, uint32_t nThreads) {
        checkThreads(nThreads);
        if (stillEmpty) {
            //Insert the data into the empty tree. This is done sequentially.
            tree.bulk_load(batch.begin(), batch.end());
            stillEmpty = false;
        } else {
            //Insert the batch in parallel.
            insertBatch(batch, numberOfThreads);
        }
    }

    //Do not use for timing!
    void insertBatch(ValueType *batch, size_t batchSize, uint32_t nThreads) {
        std::vector<ValueType> vectorBatch(batchSize);
        vectorBatch.assign(batch, batch + batchSize);
        //Execute batch: Insert the batch into the tree
        insertBatch(vectorBatch, nThreads);
    }

    void removeBatch(ValueType *batch, size_t batchSize, uint32_t nThreads) {
        (void) batch;
        (void) batchSize;
    }

    void insertBatch(std::vector<ValueType> batch, uint32_t nThreads) {
        //Execute batch: Insert the batch into the tree
        checkThreads(nThreads);
        bulkInserter(tree, batch);
    }

    void removeBatch(std::vector<ValueType> batch) {
        (void) batch;
    }

    /**
     * Constructs the pointers of the linked list in the lowest level.
     * Needed to perform any range queries later on.
     */
    void fixLinkedList() {
        tree.fix_leaf_links();
    }

    template<typename EVALUATOR>
    void rangeQuery(ConstReference left, ConstReference right, EVALUATOR f) const {//TODO make const!
        auto current = tree.lower_bound(left);

        while (current != tree.end() && *current < right) {
            AssertMsg(left <= *current && *current < right, "Range query considers an invalid value");
            f(*current);
            ++current;
        }
    }

private:
    void checkThreads(uint32_t nThreads) {
        if (nThreads != numberOfThreads) {
            std::cout << "Error: Changed number of threads on parallel bst" << std::endl;
            std::terminate();
        }
    }

public:
    size_t size() {
        return tree.size_slow();
    }

    int64_t numberOfElements() {
        return size();
    }

public:
    static const std::string type() {
        return "parallelBst";
    }
    void printContent() {}
    void printStats() {}
    bool isValid() { return true; }
};