#include "batchedSparseArray.hpp"
#include "overallocatedBatchedSparseArray.hpp"
#include "thread_coordination.hpp"
#include "definitions.hpp"
#include "operationsGenerator.hpp"
#include "prefillGenerator.hpp"
#include "benchmarks.hpp"
#include "configuration.hpp"
#include "timer.hpp"
#include "randomDistributions.hpp"
#include "cpmaWrapper.hpp"
#include "parallelBstWrapper.h"
#include "densityManager.hpp"

#define _MSC_VER false
#define TBB_ALLOCATOR_TRAITS_BROKEN 1
#include "tbb/scalable_allocator.h"

#include <iostream>
#include <string>
#include <random>
#define USE_PROFILER false

using AllocatorType = tbb::scalable_allocator<Benchmark::DataType>;

#if USE_PROFILER
using SearchDataStructure = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 1024, DataStructures::DensityOne, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureScan = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 256, DataStructures::DensityOne, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;
using CpmaDataStructure = CpmaWrapper<Benchmark::DataType, Helpers::Profiler::SequentialProfiler>;
using ParallelBSTDataStructure = ParallelBST<Benchmark::DataType, Helpers::Profiler::SequentialProfiler>;
using SearchDataStructureOverallocated = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, true, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;
#else
using SearchDataStructure = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureScan = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using CpmaDataStructure = CpmaWrapper<Benchmark::DataType, Helpers::Profiler::NoProfiler>;
using ParallelBSTDataStructure = ParallelBST<Benchmark::DataType, Helpers::Profiler::NoProfiler>;
using SearchDataStructureOverallocated = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
#endif

static const uint64_t MinKey = 1;

struct Query {
    Benchmark::DataType start;
    Benchmark::DataType end;
};

Helpers::PrefillGenerator::BatchGenerator<Benchmark::DataType> prefillGen;
Helpers::OperationsGenerator::BatchGenerator<Benchmark::DataType> opGen;
size_t numberOfQueries;
Benchmark::RandomEngine rng;
std::vector<Query> queries;

template<typename T>
void runPrefill(T &sds, Helpers::Configuration<Benchmark::DataType> config) {
    for (auto op : prefillGen.prefillOps) {
        sds.insertBatch(op.batch.start, op.batch.size, config.numThreads);
    }
    std::cout << "Inserted all prefill for sds type " << sds.type() << std::endl;
    sds.printStats();
}

template<typename T>
void runBenchmark(T &sds, Helpers::Configuration<Benchmark::DataType> config) {
#if USE_PROFILER
    sds.profiler.activate();
#endif
    std::cout << "Running the queries for data structure type " << sds.type() << std::endl;
    //Run the queries
    Helpers::Timer timer{};
    size_t querySum = 0;
    int i = 0;
    for (const Query &query : queries) {
        size_t queryLength = 0;
        asm volatile("" : : "g"(querySum) : "memory");
        timer.restart();
        sds.rangeQuery(query.start, query.end, [&queryLength, &querySum](const Benchmark::DataType &key) {
            ++queryLength;
            querySum += key;
        });
        const size_t queryTime = timer.getNanoseconds();
        asm volatile("" : : : "memory");

        std::cout << "RESULT"
                  << " queryLength=" << std::fixed << std::setprecision(3) << queryLength
                  << " queryTime=" << std::fixed << std::setprecision(3) << queryTime
                  << " querySum=" << std::fixed << std::setprecision(3) << querySum
                  << " queryStart=" << query.start
                  << " queryEnd=" << query.end
                  << " numberOfQueries=" << numberOfQueries
                  << " timeUnit=nanoseconds"
                  << " sdsType=" << sds.type()
                  << " " << config
                  << " benchmark=rangeQuery "
                  << std::endl;
#if USE_PROFILER
        std::string queryInformation = " queryLength=" +  std::to_string(queryLength)
                                            + " queryTime=" + std::to_string(queryTime)
                                            + " querySum=" + std::to_string(querySum)
                                            + " queryStart=" + std::to_string(query.start)
                                            + " queryEnd=" + std::to_string(query.end)
                                            + " numberOfQueries=" + std::to_string(numberOfQueries)
                                            + " timeUnit=microseconds";
        sds.profiler.printResultLines(config, "rangeQuery", sds.type(), i, queryInformation, 0, 2, 0, 0);
        sds.profiler.reset();
#endif
        ++i;
    }
}

int main(int argc, char *argv[]) {
    Helpers::Configuration<Benchmark::DataType> config(argc, argv);
    std::cout << "Running a range query benchmark with configuration: " << config << std::endl;
    numberOfQueries = config.numOperations;
    config.numOperations = 0;//Do not execute any additional queries.
    //Fill the data structure
    rng.seed(config.seed);
    opGen.generateOperations(config, rng);
    AssertMsg(opGen.ops.size() == 0, "Operations were generated.");
    prefillGen.generatePrefill(config, rng);
    AllocatorType alloc;
    auto batchedSparseArray = SearchDataStructure{alloc, config.numThreads};
    auto batchedSparseArrayScan = SearchDataStructureScan{alloc, config.numThreads};
    auto cpma = CpmaDataStructure{config.numThreads};
    auto bst = ParallelBSTDataStructure{config.numThreads};
    auto overallocatedSparseArray = SearchDataStructureOverallocated{alloc, config.numThreads};
    std::cout << "Generated operations and data structures." << std::endl;

    //Prepare the queries
    Helpers::UniformDistribution startDistribution(config.minKey, config.maxKey / 2);
    Helpers::ZipfDistribution keyDifferenceDistribution(config.minKey, config.maxKey / 10);
    queries.resize(numberOfQueries);
    for (Query &query : queries) {
        query.start = startDistribution(rng);
        query.end = query.start + keyDifferenceDistribution(rng);
    }
    std::cout << "Generated queries." << std::endl;

    auto prefillRunner = [config](auto&&... sds) {
        (runPrefill(sds, config), ...);
    };

    auto experimentRunner = [config](auto&&... sds) {
        (runBenchmark(sds, config), ...);
    };

    std::cout << "Prefilling the data structures with #operations: " << prefillGen.prefillOps.size() << std::endl;
    prefillRunner(batchedSparseArray, batchedSparseArrayScan, cpma, overallocatedSparseArray);
    std::cout << "Prefilled batch pma and cpma." << std::endl;
    //Manually insert into bst, as it needs another function.
    for (size_t i = 0; i < prefillGen.prefillOps.size(); ++i) {
        if (i == 0) {
            //This is needed to make the bst work... Does not change anything about the elements.
            std::vector<Benchmark::DataType> vectorBatch(config.batchSize);
            vectorBatch.assign(prefillGen.prefillOps[0].batch.start, prefillGen.prefillOps[0].batch.start + prefillGen.prefillOps[0].batch.size);
            bst.insertPrefillBatch(vectorBatch, config.numThreads);
        } else {
            bst.insertBatch(prefillGen.prefillOps[i].batch.start, prefillGen.prefillOps[i].batch.size, config.numThreads);
        }
    }
    std::cout << "Prefilled all the data structures." << std::endl;

    bst.fixLinkedList();
    std::cout << "Fixed the linked list of the bst." << std::endl;

    experimentRunner(batchedSparseArray, batchedSparseArrayScan, cpma, bst, overallocatedSparseArray);
    return 0;
}
