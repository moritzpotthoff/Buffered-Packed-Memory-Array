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

namespace Helpers::InputGenerator {
    template<typename DataType>
    class BatchGenerator {
    public:
        BatchGenerator(Helpers::Configuration<DataType> config, Definitions::RandomEngine &rng) {
            const size_t totalNumOperations = config.numOperations + config.numPrefill / config.batchSize + 1;
            if (config.keyDistribution == Helpers::ASCENDING_STAR) {
                //Special case, use one distribution for both
                Helpers::DistributionWrapper<DataType> *combinedDistribution = new Helpers::DistributionWrapper<DataType>(Helpers::ASCENDING, config.minKey, config.maxKey, totalNumOperations, config.batchSize, rng);
                prefillKeyGen = combinedDistribution;
                opKeyGen = combinedDistribution;
            } else if (config.keyDistribution == Helpers::DESCENDING_STAR) {
                //Special case, use one distribution for both
                Helpers::DistributionWrapper<DataType> *combinedDistribution = new Helpers::DistributionWrapper<DataType>(Helpers::DESCENDING, config.minKey, config.maxKey, totalNumOperations, config.batchSize, rng);
                prefillKeyGen = combinedDistribution;
                opKeyGen = combinedDistribution;
            } else {
                prefillKeyGen = new Helpers::DistributionWrapper<DataType>(Helpers::UNIFORM, config.minKey, config.maxKey, config.numPrefill / config.batchSize + 1, config.batchSize, rng);
                opKeyGen = new Helpers::DistributionWrapper<DataType>(config.keyDistribution, config.minKey, config.maxKey, config.numOperations, config.batchSize, rng);
            }
            generatePrefill(config.numPrefill, prefillKeyGen, rng, config.batchSize);
            generateOperations(config.numOperations, config.insertWeight, config.removeWeight, config.searchWeight, opKeyGen, rng, config.batchSize);
        }

        void generatePrefill(std::size_t numPrefill, auto &keyGen, Definitions::RandomEngine &rng, size_t batchSize = 1024) {
            const size_t numberOfOperations = numPrefill / batchSize;
            prefillOps.resize(numberOfOperations);
            for (size_t i = 0; i < numberOfOperations; ++i) {
                prefillOps[i] = Definitions::Operation<DataType>(Definitions::OperationType::BatchInsert, keyGen->generateBatchWithoutDuplicates(rng));
                AssertMsg(prefillOps[i].type == Definitions::OperationType::BatchInsert, "Wrong insertion type generated.");
            }
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
                                  return Definitions::Operation(op, keyGen->operator()(rng));
                              } else if (op == Definitions::OperationType::BatchInsert || op == Definitions::OperationType::BatchRemove){
                                  return Definitions::Operation<DataType>(op, keyGen->generateBatchWithoutDuplicates(rng));
                              } else {
                                  AssertMsg(false, "Not implemented");
                                  std::terminate();
                              }
                          });
        }

        Helpers::DistributionWrapper<DataType> *prefillKeyGen;
        Helpers::DistributionWrapper<DataType> *opKeyGen;
        std::vector<Definitions::Operation<DataType>> prefillOps;
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