#pragma once

#include <random>
#include <map>
#include <algorithm>

#include "configuration.hpp"
#include "zipfian_int_distribution.h"
#include "definitions.hpp"

namespace Helpers {

enum KeyDistribution {
    UNIFORM,
    SKEWED_UNIFORM,
    NORMAL,
    DENSE_NORMAL,
    ZIPF,
    ASCENDING,
    DESCENDING,
    ASCENDING_STAR,
    DESCENDING_STAR,
    numberOfKeyDistributions//Must be last.
};

constexpr const char* KeyDistributionNames[] = {
        "uniform",
        "skewedUniform",
        "normal",
        "denseNormal",
        "zipf",
        "ascending",
        "descending",
        "ascendingStar",
        "descendingStar",
};

static std::map<uint64_t, double> precomputedZetaValues{
        {1000, 7.72895},
        {10000, 10.2244},
        {50000, 12.0033},
        {100000, 12.7783},
        {1000000, 15.3918},
        {10000000, 18.0662},
        {100000000, 20.8029},
        {1000000000, 23.6034},
        {10000000000, 26.469},
        {100000000000, 29.4014},
        {1000000000000, 32.4022},
        {10000000000000, 35.4726},
};

template<typename KEY_TYPE>
class UniformDistribution {
private:
    std::uniform_int_distribution<KEY_TYPE> distribution;
public:
    UniformDistribution(KEY_TYPE minKey, KEY_TYPE maxKey) : distribution(minKey, maxKey) {}

    KEY_TYPE operator() (Definitions::RandomEngine &rng) {
        return distribution(rng);
    }
};

template<typename KEY_TYPE>
class NormalDistribution {
private:
    static constexpr double MeanPosition = 0.5;
    static constexpr double RelativeStandardDeviation = 0.02;

private:
    KEY_TYPE min;
    KEY_TYPE max;
    std::normal_distribution<double> distribution;

public:
    NormalDistribution(KEY_TYPE minKey, KEY_TYPE maxKey) :
        min(minKey),
        max(maxKey),
        distribution(static_cast<double>(min) + MeanPosition * static_cast<double>(max - min), RelativeStandardDeviation * static_cast<double>(max - min)) {}

    KEY_TYPE operator() (Definitions::RandomEngine &rng) {
        KEY_TYPE guess = 0;
        while (guess < min || guess > max) guess = static_cast<KEY_TYPE>(distribution(rng));
        return guess;
    }
};

template<typename KEY_TYPE>
class DenseNormalDistribution {
private:
    static constexpr double MeanPosition = 0.5;
    static constexpr double RelativeStandardDeviation = 0.002;

private:
    KEY_TYPE min;
    KEY_TYPE max;
    std::normal_distribution<double> distribution;

public:
    DenseNormalDistribution(KEY_TYPE minKey, KEY_TYPE maxKey) :
            min(minKey),
            max(maxKey),
            distribution(static_cast<double>(min) + MeanPosition * static_cast<double>(max - min), RelativeStandardDeviation * static_cast<double>(max - min)) {}

    KEY_TYPE operator() (Definitions::RandomEngine &rng) {
        KEY_TYPE guess = 0;
        while (guess < min || guess > max) guess = static_cast<KEY_TYPE>(distribution(rng));
        return guess;
    }
};

template<typename KEY_TYPE>
class ZipfDistribution {
    using ZipfDistributionType = zipfian_int_distribution<KEY_TYPE>;
private:
    ZipfDistributionType distribution;
    static constexpr double Theta = 0.99;
public:
    ZipfDistribution(KEY_TYPE minKey, KEY_TYPE maxKey) {
        typename ZipfDistributionType::param_type parameters(minKey, maxKey, Theta, getZeta(minKey, maxKey));
        distribution = ZipfDistributionType(parameters);
    }

    static double computeZeta(KEY_TYPE distance) {
        std::cout << "Calculating new zeta value..." << std::endl;
        double result = 0.0;
        for (KEY_TYPE i=1; i <= distance; ++i) {
            result += std::pow(1.0 / static_cast<double>(i) , Theta);
        }
        return result;
    }

    double getZeta(KEY_TYPE minKey, KEY_TYPE maxKey) {
        if (precomputedZetaValues.contains(maxKey)) {
            return precomputedZetaValues[maxKey];
        }
        //Compute and add the new value
        double zeta = computeZeta(maxKey - minKey + 1);
        precomputedZetaValues[maxKey] = zeta;
        std::cout << "New zeta value: {" << maxKey << ", " << zeta << "}" << std::endl;
        return zeta;
    }

    KEY_TYPE operator() (Definitions::RandomEngine &rng) {
        return distribution(rng);
    }
};

template<typename KEY_TYPE>
class AscDistribution {
private:
    std::uniform_int_distribution<KEY_TYPE> distribution;
public:
    AscDistribution(KEY_TYPE minKey, KEY_TYPE maxKey) : distribution(minKey, maxKey) {}

    KEY_TYPE operator() (Definitions::RandomEngine &rng) {
        return distribution(rng);
    }
};

template<typename KEY_TYPE>
class DescDistribution {
private:
    std::uniform_int_distribution<KEY_TYPE> distribution;
public:
    DescDistribution(KEY_TYPE minKey, KEY_TYPE maxKey) : distribution(minKey, maxKey) {}

    KEY_TYPE operator() (Definitions::RandomEngine &rng) {
        return distribution(rng);
    }
};

template<typename KEY_TYPE>
class DistributionWrapper {
private:
    KeyDistribution distributionType;
    size_t maxNumberOfAccesses;//Used for ascending and descending operations
    size_t batchSize;
    size_t precomputedIndex;
    size_t numberOfPrecomputedElements;
    union {
        UniformDistribution<KEY_TYPE> uniformDistribution;
        NormalDistribution<KEY_TYPE> normalDistribution;
        DenseNormalDistribution<KEY_TYPE> denseNormalDistribution;
        ZipfDistribution<KEY_TYPE> zipfDistribution;
        AscDistribution<KEY_TYPE> ascDistribution;
        DescDistribution<KEY_TYPE> descDistribution;
    };
    KEY_TYPE *precomputed;

public:
    DistributionWrapper() {}

    DistributionWrapper(KeyDistribution keyDistribution, KEY_TYPE minKey, KEY_TYPE maxKey, size_t numberOfOperations, size_t batch, Definitions::RandomEngine &rng) :
        distributionType(keyDistribution),
        maxNumberOfAccesses(numberOfOperations),
        batchSize(batch),
        precomputedIndex(0),
        numberOfPrecomputedElements(0) {
        switch (distributionType) {
            case KeyDistribution::SKEWED_UNIFORM: {
                [[fallthrough]];//Skewed uniform is just a uniform distribution in a smaller key range
            }
            case KeyDistribution::UNIFORM: {
                uniformDistribution = UniformDistribution<KEY_TYPE>(minKey, maxKey);
                break;
            }
            case KeyDistribution::NORMAL: {
                normalDistribution = NormalDistribution<KEY_TYPE>(minKey, maxKey);
                break;
            }
            case KeyDistribution::DENSE_NORMAL: {
                denseNormalDistribution = DenseNormalDistribution<KEY_TYPE>(minKey, maxKey);
                break;
            }
            case KeyDistribution::ZIPF: {
                zipfDistribution = ZipfDistribution<KEY_TYPE>(minKey, maxKey);
                break;
            }
            case KeyDistribution::ASCENDING: {
                ascDistribution = AscDistribution<KEY_TYPE>(minKey, maxKey);
                precomputeRandomNumbers(rng);
                break;
            }
            case KeyDistribution::DESCENDING: {
                descDistribution = DescDistribution<KEY_TYPE>(minKey, maxKey);
                precomputeRandomNumbers(rng);
                break;
            }
            default: {
                std::cerr << "Unknown random distribution type" << std::endl;
                std::terminate();
            }
        }
    }

    ~DistributionWrapper() {
        if (distributionType == KeyDistribution::ASCENDING || distributionType == KeyDistribution::DESCENDING) {
            free(precomputed);
            precomputed = nullptr;
        }
    }

    KEY_TYPE* generateBatch(Definitions::RandomEngine &rng) {
        KEY_TYPE *batchStart = static_cast<KEY_TYPE *>(malloc((size_t) batchSize * sizeof(KEY_TYPE)));
        for (size_t i = 0; i < batchSize; ++i) batchStart[i] = this->operator()(rng);
        std::sort(batchStart, batchStart + batchSize);
        return batchStart;
    }

    Definitions::Batch<KEY_TYPE> generateBatchWithoutDuplicates(Definitions::RandomEngine &rng) {
        KEY_TYPE *batchStart = static_cast<KEY_TYPE *>(malloc((size_t) batchSize * sizeof(KEY_TYPE)));
        for (size_t i = 0; i < batchSize; ++i) batchStart[i] = this->operator()(rng);
        std::sort(batchStart, batchStart + batchSize);
        auto newEnd = std::unique(batchStart, batchStart + batchSize, [](const KEY_TYPE &lhs, const KEY_TYPE &rhs) -> bool {
            return lhs == rhs;
        });
        size_t actualBatchSize = static_cast<size_t>(newEnd - batchStart);
        return Definitions::Batch<KEY_TYPE>(batchStart, actualBatchSize);
    }

    void precomputeRandomNumbers(Definitions::RandomEngine &rng) {
        //Generate random numbers
        if (distributionType == KeyDistribution::ASCENDING || distributionType == KeyDistribution::DESCENDING) {
            numberOfPrecomputedElements = maxNumberOfAccesses * batchSize;
            precomputed = static_cast<KEY_TYPE*>(malloc(numberOfPrecomputedElements * sizeof(KEY_TYPE)));
            for (size_t i = 0; i < numberOfPrecomputedElements; ++i) {
                if (distributionType == KeyDistribution::ASCENDING) {
                    precomputed[i] = ascDistribution(rng);
                } else if (distributionType == KeyDistribution::DESCENDING) {
                    precomputed[i] = descDistribution(rng);
                }
            }
            //Sort them
            if (distributionType == KeyDistribution::ASCENDING) {
                std::sort(precomputed, precomputed + numberOfPrecomputedElements);
            } else if (distributionType == KeyDistribution::DESCENDING) {
                std::sort(precomputed, precomputed + numberOfPrecomputedElements, std::greater{});//Sort descending
            }
        }
    }

    KEY_TYPE operator() (Definitions::RandomEngine &rng) {
        switch (distributionType) {
            case KeyDistribution::SKEWED_UNIFORM: {
                [[fallthrough]];//Skewed uniform is just a uniform distribution in a smaller key range
            }
            case KeyDistribution::UNIFORM: {
                return uniformDistribution(rng);
                break;
            }
            case KeyDistribution::NORMAL: {
                return normalDistribution(rng);
                break;
            }
            case KeyDistribution::DENSE_NORMAL: {
                return denseNormalDistribution(rng);
                break;
            }
            case KeyDistribution::ZIPF: {
                return zipfDistribution(rng);
                break;
            }
            case KeyDistribution::ASCENDING: {
                AssertMsg(precomputedIndex < numberOfPrecomputedElements, "Read too many random numbers.");
                KEY_TYPE result = precomputed[precomputedIndex];
                ++precomputedIndex;
                return result;
                break;
            }
            case KeyDistribution::DESCENDING: {
                AssertMsg(precomputedIndex < numberOfPrecomputedElements, "Read too many random numbers.");
                KEY_TYPE result = precomputed[precomputedIndex];
                ++precomputedIndex;
                return result;
                break;
            }
            default: {
                std::cerr << "Unknown random distribution type" << std::endl;
                std::terminate();
            }
        }
    }
};
}