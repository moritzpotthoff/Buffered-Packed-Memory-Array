#include "thread_coordination.hpp"
#include "definitions.hpp"
#include "overallocatedBatchedSparseArray.hpp"
#include "inputGenerator.hpp"
#include "benchmarks.hpp"
#include "configuration.hpp"

#define _MSC_VER false
#define TBB_ALLOCATOR_TRAITS_BROKEN false
#include "tbb/scalable_allocator.h"

#include <iostream>
#include <string>
using AllocatorType = tbb::scalable_allocator<Benchmark::DataType>;
using SearchDataStructure = DataStructures::OverallocatedBatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, true, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;

static const uint64_t MinKey = 1;

template<typename T>
void runBenchmark(T &sds, Helpers::Configuration<Benchmark::DataType> config, double overallocationFactor, auto &inputGen) {
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
                  << " benchmark=batchOverallocationFactorRandomDistribution"
                  << " overallocationFactor=" << overallocationFactor
                  << " blockSize=" << sds.getBlockSize()
                  << " numberOfElements=" << sds.numberOfElements()
                  << " value=" << std::fixed << std::setprecision(3) << std::chrono::duration<double>(runner.getDuration("prefill")).count()
                  << std::endl;
        std::cout << "RESULT" << " " << config << " iteration=" << i
                  << " metric=workTime"
                  << " sdsType=" << sds.type()
                  << " overallocationFactor=" << overallocationFactor
                  << " benchmark=batchOverallocationFactorRandomDistribution"
                  << " blockSize=" << sds.getBlockSize()
                  << " numberOfElements=" << sds.numberOfElements()
                  << " value=" << std::fixed << std::setprecision(3) << std::chrono::duration<double>(runner.getDuration("work")).count()
                  << std::endl;

        std::string factorIdentifier = "overallocationFactor=" + std::to_string(overallocationFactor);
        sds.profiler.printEventLines(config, "overallocationFactorRandomDistribution", sds.type(), i, factorIdentifier);
        sds.profiler.print();
    }
}

inline void runExperiment(Helpers::Configuration<Benchmark::DataType> config, double factor, auto &inputGen) {
    std::cout << "Running a benchmark with overallocation factor " << factor << " and configuration: " << config << std::endl;
    AllocatorType alloc;
    SearchDataStructure sds{alloc, config.numThreads};
    sds.overallocationFactor = factor;
    runBenchmark(sds, config, factor, inputGen);
}

int main(int argc, char *argv[]) {
    Helpers::Configuration<Benchmark::DataType> config(argc, argv);
    Benchmark::RandomEngine rng(config.seed);
    Helpers::InputGenerator::BatchGenerator<Benchmark::DataType> inputGen(config, rng);
    for (double factor = 1.0; factor <= 2.5; factor += 0.05) {
        runExperiment(config, factor, inputGen);
    }

    return 0;
}
