#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdlib>
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

#include "definitions.hpp"
#include "configuration.hpp"

namespace Helpers::OperationsGenerator {
    template<typename DataType>
    class RandomGenerator {
    public:
        void generateOperations(Configuration<DataType> config) {
            generateOperations(config.numOperations, config.insertWeight, config.removeWeight, config.searchWeight, config.seed, 1, config.maxKey);
        }

        void generateOperations(
                size_t numOperations,
                double insertWeight, double removeWeight, double searchWeight,
                std::uint32_t seed = 1,
                DataType minElement = std::numeric_limits<DataType>::lowest(),
                DataType maxElement = std::numeric_limits<DataType>::max()) {
            Definitions::RandomEngine rng(seed);
            auto elementDist = std::uniform_int_distribution<DataType>(minElement, maxElement);
            auto opDist = std::discrete_distribution<>({insertWeight, removeWeight, searchWeight});

            ops.resize(numOperations);
            std::generate(ops.begin(), ops.end(),
                          [&elementDist, &opDist, &rng]() -> Definitions::Operation<DataType> {
                              return Definitions::Operation(static_cast<Definitions::OperationType>(opDist(rng)), elementDist(rng));
                          });
        }

        std::vector<Definitions::Operation<DataType>> ops;
    };

    template<typename DataType>
    class BatchGenerator {
    public:
        void generateOperations(Configuration<DataType> config, Definitions::RandomEngine &rng) {
            auto keyGen = config.getKeyGen(rng);
            generateOperations(config.numOperations, config.insertWeight, config.removeWeight, config.searchWeight, keyGen, rng, config.batchSize);
        }

        void generateOperations(
                size_t numOperations,
                double batchInsertWeight, double batchRemoveWeight, double searchWeight,
                auto &keyGen,
                Definitions::RandomEngine &rng,
                size_t batchSize = 1024) {
            auto opDist = std::discrete_distribution<>({0.0, 0.0, searchWeight, batchInsertWeight, batchRemoveWeight});

            ops.resize(numOperations);
            std::generate(ops.begin(), ops.end(),
                          [&keyGen, &opDist, &rng, batchSize]() -> Definitions::Operation<DataType> {
                              Definitions::OperationType op = static_cast<Definitions::OperationType>(opDist(rng));
                              if (op == Definitions::OperationType::Search) {
                                  return Definitions::Operation(op, keyGen(rng));
                              } else if (op == Definitions::OperationType::BatchInsert || op == Definitions::OperationType::BatchRemove){
                                  return Definitions::Operation<DataType>(op, keyGen.generateBatchWithoutDuplicates(rng));
                              } else {
                                  AssertMsg(false, "Not implemented");
                                  std::terminate();
                              }
                          });
        }

        std::vector<Definitions::Operation<DataType>> ops;
    };

    template<typename KeyType, typename MappedType, typename ValueType>
    class ComplexBatchGenerator {
    public:
        void generateOperations(
                size_t numOperations,
                double batchInsertWeight, double batchRemoveWeight, double searchWeight,
                auto &keyGen,
                auto &mappedGen,
                Definitions::RandomEngine &rng,
                size_t batchSize = 1024) {
            auto opDist = std::discrete_distribution<>({0.0, 0.0, searchWeight, batchInsertWeight, batchRemoveWeight});

            ops.resize(numOperations);
            std::generate(ops.begin(), ops.end(),
                          [&keyGen, &mappedGen, &opDist, &rng, batchSize]() -> Definitions::Operation<KeyType, ValueType> {
                              Definitions::OperationType op = static_cast<Definitions::OperationType>(opDist(rng));
                              if (op == Definitions::OperationType::Search) {
                                  return Definitions::Operation<KeyType, ValueType>(op, keyGen(rng));
                              } else if (op == Definitions::OperationType::BatchInsert || op == Definitions::OperationType::BatchRemove){
                                  Definitions::Batch<KeyType> keyBatch = keyGen.generateBatchWithoutDuplicates(rng);
                                  MappedType *mappedBatch = mappedGen.generateBatch(rng);
                                  ValueType *batchValues = static_cast<ValueType*>(malloc(batchSize * sizeof(ValueType)));
                                  for (size_t i = 0; i < keyBatch.size; ++i) {
                                      batchValues[i] = ValueType{keyBatch.start[i], mappedBatch[i]};
                                  }
                                  Definitions::Batch<ValueType> batch(batchValues, static_cast<int64_t>(keyBatch.size));
                                  return Definitions::Operation<KeyType, ValueType>(op, batch);
                              } else {
                                  AssertMsg(false, "Not implemented");
                                  std::terminate();
                              }
                          });
        }

        std::vector<Definitions::Operation<KeyType, ValueType>> ops;
    };
}