#pragma once

#include "definitions.hpp"

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

#include "definitions.hpp"
#include "configuration.hpp"

namespace Helpers::PrefillGenerator {
    template<typename DataType>
    class UniformGenerator {
    public:
        void generatePrefill(Configuration<DataType> config) {
            generatePrefill(config.numPrefill, config.seed, 1, config.maxKey);
        }

        void generatePrefill(
                std::size_t numPrefill,
                std::uint32_t seed = 1,
                DataType min_element = std::numeric_limits<DataType>::lowest(),
                DataType max_element = std::numeric_limits<DataType>::max()) {
            Definitions::RandomEngine rng(seed);
            auto elementDist = std::uniform_int_distribution<DataType>(min_element, max_element);
            prefill.resize(numPrefill);
            std::generate(prefill.begin(), prefill.end(), [&elementDist, &rng]() { return elementDist(rng); });
        }
        std::vector<DataType> prefill;
    };

    template<typename DataType>
    class BatchGenerator {
    public:
        void generatePrefill(Configuration<DataType> config, Definitions::RandomEngine &rng) {
            auto keyGen = config.getPrefillKeyGen(rng);
            generatePrefill(config.numPrefill, keyGen, rng, config.batchSize);
        }

        void generatePrefill(
                std::size_t numPrefill,
                auto &keyGen,
                Definitions::RandomEngine &rng,
                size_t batchSize = 1024) {
            const size_t numberOfOperations = numPrefill / batchSize;
            prefillOps.resize(numberOfOperations);
            for (size_t i = 0; i < numberOfOperations; ++i) {
                //Definitions::Batch<DataType> batch(keyGen.generateBatch(rng), static_cast<int64_t>(batchSize));
                prefillOps[i] = Definitions::Operation<DataType>(Definitions::OperationType::BatchInsert, keyGen.generateBatchWithoutDuplicates(rng));
            }
        }
        std::vector<Definitions::Operation<DataType>> prefillOps;
    };
}