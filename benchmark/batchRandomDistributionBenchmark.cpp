#include "batchedSparseArray.hpp"
#include "overallocatedBatchedSparseArray.hpp"
#include "thread_coordination.hpp"
#include "definitions.hpp"
#include "inputGenerator.hpp"
#include "benchmarks.hpp"
#include "configuration.hpp"
#include "timer.hpp"
#include "densityManager.hpp"

#define _MSC_VER false
#define TBB_ALLOCATOR_TRAITS_BROKEN false
#include "tbb/scalable_allocator.h"

#include <iostream>
#include <string>
#include <random>
#define USE_PROFILER false

using AllocatorType = tbb::scalable_allocator<Benchmark::DataType>;
//using AllocatorType = std::allocator<Benchmark::DataType>;

#if USE_PROFILER
using SearchDataStructure = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 1024, DataStructures::DensityOne, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;
//using SearchDataStructure = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, true, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;
#else
//Scan configuration:
//using SearchDataStructure = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
//Insertion configuration
using SearchDataStructure = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
//Overallocated variant
//using SearchDataStructure = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
#endif

static const uint64_t MinKey = 1;

int main(int argc, char *argv[]) {
    Helpers::Configuration<Benchmark::DataType> config(argc, argv);
    std::cout << "Running a benchmark with configuration: " << config << std::endl;
    Benchmark::RandomEngine rng;
    rng.seed(config.seed);
    Helpers::InputGenerator::BatchGenerator<Benchmark::DataType> inputGen(config, rng);

    std::cout << "iteration, prefill time, work time, final number of elements, valid data structure?\n";
    for (auto i = 0; i < config.iterations; ++i) {
        AllocatorType alloc;
        auto sds = SearchDataStructure{alloc, config.numThreads};
        Benchmark::BatchBenchmark<SearchDataStructure> runner{};
        runner.run(std::ref(sds), std::ref(inputGen.prefillOps), std::ref(inputGen.ops), config.numThreads);

        std::cout << i << ',' << std::fixed << std::setprecision(3)
                  << std::chrono::duration<double>(runner.getDuration("prefill")).count()
                  << ',' << std::fixed << std::setprecision(3)
                  << std::chrono::duration<double>(runner.getDuration("work")).count()
                  << ", " << sds.numberOfElements()
                  << ", " << sds.isValid()
                  << '\n';

        std::cout << "RESULT" << " " << config << " iteration=" << i
                  << " metric=prefillTime"
                  << " sdsType=" << sds.type()
                  << " benchmark=randomDistribution"
                  << " numberOfElements=" << sds.numberOfElements()
                  << " value=" << std::fixed << std::setprecision(3) << std::chrono::duration<double>(runner.getDuration("prefill")).count()
                  << std::endl;
        std::cout << "RESULT" << " " << config << " iteration=" << i
                  << " metric=workTime"
                  << " sdsType=" << sds.type()
                  << " benchmark=randomDistribution"
                  << " numberOfElements=" << sds.numberOfElements()
                  << " value=" << std::fixed << std::setprecision(3) << std::chrono::duration<double>(runner.getDuration("work")).count()
                  << std::endl;

        sds.printStats();
#if USE_PROFILER
        sds.profiler.printResultLines(config, "randomDistribution", sds.type(), i);
        sds.profiler.print();
#endif
        //sds.debugStream.print();
        //sds.printBlockStructure();
    }
    return 0;
}
