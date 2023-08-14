#include "overallocatedBatchedSparseArray.hpp"
#include "inputGenerator.hpp"
#include "thread_coordination.hpp"
#include "definitions.hpp"
#include "benchmarks.hpp"
#include "configuration.hpp"

#define _MSC_VER false
#define TBB_ALLOCATOR_TRAITS_BROKEN false
#include "tbb/scalable_allocator.h"

#include <iostream>
#include <string>

using AllocatorType = tbb::scalable_allocator<Benchmark::DataType>;
using SearchDataStructure16 = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructure32 = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructure64 = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructure128 = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructure256 = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructure512 = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructure1024 = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructure2048 = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructure4096 = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, true, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

static const uint64_t MinKey = 1;

template<typename T>
void runBenchmark(T &sds, Helpers::Configuration<Benchmark::DataType> config, auto &inputGen) {
    std::cout << "iteration, prefill time, work time, final number of elements, valid data structure?\n";
    for (auto i = 0; i < config.iterations; ++i) {
        Benchmark::BatchBenchmark<T> runner{};
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
                  << " benchmark=batchBlockSizeRandomDistribution"
                  << " blockSize=" << sds.getBlockSize()
                  << " numberOfElements=" << sds.numberOfElements()
                  << " value=" << std::fixed << std::setprecision(3) << std::chrono::duration<double>(runner.getDuration("prefill")).count()
                  << std::endl;
        std::cout << "RESULT" << " " << config << " iteration=" << i
                  << " metric=workTime"
                  << " sdsType=" << sds.type()
                  << " benchmark=batchBlockSizeRandomDistribution"
                  << " blockSize=" << sds.getBlockSize()
                  << " numberOfElements=" << sds.numberOfElements()
                  << " value=" << std::fixed << std::setprecision(3) << std::chrono::duration<double>(runner.getDuration("work")).count()
                  << std::endl;
    }
}

int main(int argc, char *argv[]) {
    Helpers::Configuration<Benchmark::DataType> config(argc, argv);
    std::cout << "Running a benchmark with configuration: " << config << std::endl;
    Benchmark::RandomEngine rng(config.seed);

    Helpers::InputGenerator::BatchGenerator<Benchmark::DataType> inputGen(config, rng);;
    auto blockRunner = [config, &inputGen](auto&&... sds) {
        (runBenchmark(sds, config, inputGen), ...);
    };

    AllocatorType alloc;
    SearchDataStructure16 sds16{alloc, config.numThreads};
    SearchDataStructure32 sds32{alloc, config.numThreads};
    SearchDataStructure64 sds64{alloc, config.numThreads};
    SearchDataStructure128 sds128{alloc, config.numThreads};
    SearchDataStructure256 sds256{alloc, config.numThreads};
    SearchDataStructure512 sds512{alloc, config.numThreads};
    SearchDataStructure1024 sds1024{alloc, config.numThreads};
    SearchDataStructure2048 sds2048{alloc, config.numThreads};
    SearchDataStructure4096 sds4096{alloc, config.numThreads};

    blockRunner(sds16, sds32, sds64, sds128, sds256, sds512, sds1024, sds2048, sds4096);

    return 0;
}
