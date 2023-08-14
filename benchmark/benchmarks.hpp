#pragma once

#include "thread_coordination.hpp"
#include "definitions.hpp"
#include "timer.hpp"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <numeric>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>
#include <cstdint>

namespace Benchmark {
    using DataType = std::uint64_t;
    using RandomEngine = std::mt19937_64;

    struct KeyOf {
        static constexpr DataType keyOfValue(DataType value) {
            return value;
        }
    };

    template<typename SearchDataStructure>
    class SequentialBenchmark {
    public:
        void run(SearchDataStructure &sds, std::vector<DataType> &prefill, std::vector<Definitions::Operation<DataType>> &ops) {
            executeTimed("prefill", [&sds, &prefill]() {
                for (auto e : prefill) {
                    sds.insert(e);
                }
            });

            //prefill is done, activate the profiler only now.
            sds.profiler.activate();

            executeTimed("work", [&sds, &ops]() {
                        for (auto op : ops) {
                            switch (op.type) {
                                case Definitions::OperationType::Insert: {
                                    sds.insert(op.value);
                                    break;
                                }
                                case Definitions::OperationType::Remove: {
                                    sds.remove(op.value);
                                    break;
                                }
                                case Definitions::OperationType::Search: {
                                    bool success;
                                    // Let the success variable "escape"
                                    asm volatile(""::"g"(&success));
                                    // Adapt to your interface
                                    success = sds.search(op.value);
                                    // "Use" memory to force write to retval
                                    asm volatile("":: : "memory");
                                    break;
                                }
                                case Definitions::OperationType::BatchInsert:
                                case Definitions::OperationType::BatchRemove: {
                                    std::terminate();
                                    break;
                                }
                            }
                        }
                    });
        }

        auto getDuration(std::string const& name) {
            return times.at(name);
        }

        template <typename Work, typename... Args>
        void executeTimed(std::string const& name, Work work, Args&&... args) {
            timer.restart();
            work(std::forward<Args>(args)...);
            times[name] = timer.getTime();
        }

    public:
        std::unordered_map<std::string, Helpers::Timer::clockType::duration> times;
        Helpers::Timer timer;
    };

    template<typename SearchDataStructure>
    class BatchBenchmark : public SequentialBenchmark<SearchDataStructure> {
    public:
        void run(SearchDataStructure &sds, std::vector<Definitions::Operation<DataType>> &prefill, std::vector<Definitions::Operation<DataType>> &ops, std::uint64_t numberOfThreads) {
#pragma omp parallel for schedule(static, 1) num_threads(numberOfThreads)
            for (uint64_t i = 0; i < numberOfThreads; ++i) {
                //sds.debugStream.registerThread();
                sds.profiler.registerThread();
            }

            SequentialBenchmark<SearchDataStructure>::executeTimed("prefill", [&sds, &prefill, &numberOfThreads]() {
                for (auto op : prefill) {
                    AssertMsg(op.type == Definitions::OperationType::BatchInsert, "Wrong prefill operation.");
                    sds.insertBatch(op.batch.start, op.batch.size, numberOfThreads);
                }
            });

            //prefill is done, activate the profiler only now.
            sds.profiler.activate();

            SequentialBenchmark<SearchDataStructure>::executeTimed("work", [&sds, &ops, &numberOfThreads]() {
                for (auto op : ops) {
                    switch (op.type) {
                        case Definitions::OperationType::Insert: {
                            std::terminate();
                            break;
                        }
                        case Definitions::OperationType::Remove: {
                            std::terminate();
                            break;
                        }
                        case Definitions::OperationType::Search: {
                            bool success;
                            // Let the success variable "escape"
                            asm volatile(""::"g"(&success));
                            // Adapt to your interface
                            success = sds.contains(op.value);
                            // "Use" memory to force write to retval
                            asm volatile("":: : "memory");
                            break;
                        }
                        case Definitions::OperationType::BatchInsert: {
                            sds.insertBatch(op.batch.start, op.batch.size, numberOfThreads);
                            break;
                        }
                        case Definitions::OperationType::BatchRemove: {
                            sds.removeBatch(op.batch.start, op.batch.size, numberOfThreads);
                            break;
                        }
                    }
                }
            });
        }
    };

    template<typename SearchDataStructure>
    class ParallelBSTBatchBenchmark : public SequentialBenchmark<SearchDataStructure> {
    public:
        void run(SearchDataStructure &sds, std::vector<Definitions::Operation<DataType>> &prefill, std::vector<Definitions::Operation<DataType>> &ops, std::uint64_t numberOfThreads) {
            //Use vector-based format for ParallelBST
            std::vector<std::vector<DataType>> prefillBatches(prefill.size());
            for (size_t i = 0; i < prefill.size(); ++i) {
                //Copy the batches to vectors
                prefillBatches[i].reserve(prefill[i].batch.size);
                prefillBatches[i].assign(prefill[i].batch.start, prefill[i].batch.start + prefill[i].batch.size);
            }
            std::vector<std::vector<DataType>> workBatches(ops.size());
            for (size_t i = 0; i < ops.size(); ++i) {
                if (ops[i].type != Definitions::OperationType::BatchInsert && ops[i].type != Definitions::OperationType::BatchInsert) {
                    continue;//No batch involved
                }
                workBatches[i].reserve(ops[i].batch.size);
                workBatches[i].assign(ops[i].batch.start, ops[i].batch.start + ops[i].batch.size);
            }

#pragma omp parallel for schedule(static, 1) num_threads(numberOfThreads)
            for (uint64_t i = 0; i < numberOfThreads; ++i) {
                //sds.debugStream.registerThread();
                sds.profiler.registerThread();
            }

            SequentialBenchmark<SearchDataStructure>::executeTimed("prefill", [&sds, &prefillBatches, &numberOfThreads]() {
                for (auto batch : prefillBatches) {
                    sds.insertPrefillBatch(batch, numberOfThreads);
                }
            });

            //prefill is done, activate the profiler only now.
            sds.profiler.activate();

            SequentialBenchmark<SearchDataStructure>::executeTimed("work", [&sds, &ops, &numberOfThreads, &workBatches]() {
                for (size_t i = 0; i < ops.size(); ++i) {
                    const auto op = ops[i];
                    switch (op.type) {
                        case Definitions::OperationType::Insert: {
                            std::terminate();
                            break;
                        }
                        case Definitions::OperationType::Remove: {
                            std::terminate();
                            break;
                        }
                        case Definitions::OperationType::Search: {
                            bool success;
                            // Let the success variable "escape"
                            asm volatile(""::"g"(&success));
                            // Adapt to your interface
                            success = sds.contains(op.value);
                            // "Use" memory to force write to retval
                            asm volatile("":: : "memory");
                            break;
                        }
                        case Definitions::OperationType::BatchInsert: {
                            sds.insertBatch(workBatches[i], numberOfThreads);
                            break;
                        }
                        case Definitions::OperationType::BatchRemove: {
                            sds.removeBatch(workBatches[i]);
                            break;
                        }
                    }
                }
            });
        }
    };
}
