#pragma once

#include <ostream>
#include "helpers.hpp"
#include "assert.hpp"
#include "profiler.hpp"

#include "CPMA-test-repo/CPMA.hpp"

template <typename KEY, typename PROFILER>
class CpmaWrapper {
public:
    using KeyType = KEY;
    using Reference = KEY&;
    using ConstReference = KEY const&;
    using Pointer = KeyType*;

    using Leaf = uncompressed_leaf<KeyType>;
    static constexpr HeadForm head_form = InPlace;
    static constexpr uint64_t B_size = 0;
    static constexpr bool store_density = false;
    using CpmaTraits = PMA_traits<Leaf, head_form, B_size, store_density>;
    using CpmaType = CPMA<CpmaTraits>;

    using Profiler = PROFILER;
    static constexpr bool Profile = std::is_same_v<PROFILER, Helpers::Profiler::SequentialProfiler>;

public:
    Profiler profiler;

private:
    CpmaType cpma;

public:
    CpmaWrapper(uint64_t expectedNumberOfThreads) {
#if !PARALLEL
        std::cerr << "CPMA not parallelized" << std::endl;
        std::terminate();
#endif
        if (cpma.number_of_threads() != expectedNumberOfThreads) {
            std::cerr << "Wrong number of threads " << cpma.number_of_threads() << ", expecting " << expectedNumberOfThreads << std::endl;
            std::terminate();
        }
    }

    bool contains(ConstReference e) {
        //TODO implement
        return false;
    }

    void insertBatch(Pointer batch, int64_t batchSize, uint64_t numberOfThreads) {
        cpma.insert_batch_sorted(batch, batchSize);
        (void) numberOfThreads;
    }

    void removeBatch(Pointer batch, int64_t batchSize, uint64_t numberOfThreads) {
        (void) batch;
        (void) batchSize;
        (void) numberOfThreads;
    }

    template<typename EVALUATOR>
    void rangeQuery(KeyType left, KeyType right, EVALUATOR f) const {
        cpma.map_range(f, left, right);
    }

public:
    size_t size() {
        return numberOfElements();
    }

    int64_t numberOfElements() {
        return cpma.get_element_count();
    }

public:
    static const std::string type() {
        return "cpma-uncompressed-inPlace";
    }
    void printContent() {}
    void printStats() {}
    bool isValid() { return true; }
};