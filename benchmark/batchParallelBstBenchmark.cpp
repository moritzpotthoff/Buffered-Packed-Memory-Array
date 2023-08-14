#include "definitions.hpp"
#include "inputGenerator.hpp"
#include "benchmarks.hpp"
#include "configuration.hpp"
#include "timer.hpp"
#include "helpers.hpp"
#include "assert.hpp"
#include "profiler.hpp"

#include "parallelBstWrapper.h"

#include <iostream>
#include <string>
#include <random>
#define USE_PROFILER false

using SearchDataStructure = ParallelBST<Benchmark::DataType, Helpers::Profiler::NoProfiler>;

static const uint64_t MinKey = 1;

int main(int argc, char *argv[]) {
    Helpers::Configuration<Benchmark::DataType> config(argc, argv);
    std::cout << "Running a benchmark with configuration: " << config << std::endl;
    Benchmark::RandomEngine rng(config.seed);
    Helpers::InputGenerator::BatchGenerator<Benchmark::DataType> inputGen(config, rng);

    std::cout << "iteration, prefill time, work time, final number of elements, valid data structure?\n";
    for (auto i = 0; i < config.iterations; ++i) {
        auto sds = SearchDataStructure{config.numThreads};
        Benchmark::ParallelBSTBatchBenchmark<SearchDataStructure> runner{};
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
    }
    return 0;
}
