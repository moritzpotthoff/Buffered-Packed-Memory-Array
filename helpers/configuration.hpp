#pragma once
#include <iostream>
#include <string>
#include <functional>

#include "randomDistributions.hpp"
#include "cmake_config.hpp"
#include "definitions.hpp"

namespace Helpers {
template<typename DataType>
class Configuration {
public:
    int iterations;//Number of iterations the benchmark will be run for
    size_t numPrefill;//Number of elements that are initially inserted
    size_t numOperations;//Number of operations that are executed
    unsigned int numThreads;
    double insertWeight;
    double removeWeight;
    double searchWeight;
    size_t batchSize;
    uint32_t seed;//Seed for the random generators
    uint64_t minKey;
    uint64_t maxKey;//Maximum key of elements
    KeyDistribution keyDistribution;

    static constexpr unsigned int numberOfParameters = 11;
public:
    Configuration() : iterations(10),
        numPrefill(100000),
        numOperations(100000),
        numThreads(4),
        insertWeight(0.5),
        removeWeight(0.2),
        searchWeight(0.3),
        batchSize(1024),
        seed(1),
        minKey(1),
        maxKey(std::numeric_limits<DataType>::max()),
        keyDistribution(UNIFORM) {
    }

    /**
     * Extract a configuration from the command line.
     * Parameters:
     * 1. iterations
     * 2. key generator identifier
     * 3. prefill
     * 4. operations
     * 5. threads
     * 6. insert weight
     * 7. remove weight
     * 8. search weight
     * 9. batch size
     * 10. Seed
     * 11. Max Key Value
     */
    Configuration(int argc, char *argv[]) : Configuration() {
        if (argc != numberOfParameters + 1) {
            std::cout << "No configuration provided, using the standard configuration instead." << std::endl;
            return;
        };

        iterations = std::stoi(argv[1]);
        std::string keyDistributionIdentifier = argv[2];
        //Find the key distribution:
        bool found = false;
        for (size_t distribution = 0; distribution < numberOfKeyDistributions; ++distribution) {
            if (keyDistributionIdentifier.compare(KeyDistributionNames[distribution]) == 0) {
                keyDistribution = static_cast<KeyDistribution>(distribution);
                found = true;
                break;
            }
        }
        if (!found) {
            std::cerr << "Invalid key distribution identifier, aborting.";
            std::terminate();
        }
        numPrefill = std::stoul(argv[3]);
        numOperations = std::stoul(argv[4]);
        numThreads = (unsigned int)std::stoi(argv[5]);
        insertWeight = std::stod(argv[6]), removeWeight = std::stod(argv[7]), searchWeight = std::stod(argv[8]);
        batchSize = (uint64_t)std::stoi(argv[9]);
        seed = (uint32_t)std::stoi(argv[10]);
        minKey = 1;
        maxKey = (uint64_t)std::stoll(argv[11]);
    }

    std::pair<std::reference_wrapper<Helpers::DistributionWrapper<DataType>>, std::reference_wrapper<Helpers::DistributionWrapper<DataType>>> getInputGen(Definitions::RandomEngine &rng) {
        const size_t totalNumOperations = numOperations + numPrefill / batchSize + 1;
        if (keyDistribution == Helpers::ASCENDING_STAR) {
            std::cout << "Generating distribution for ascending*" << std::endl;
            //Special case, use one distribution for both
            Helpers::DistributionWrapper<DataType> combinedDistribution(Helpers::ASCENDING, minKey, maxKey, totalNumOperations, batchSize, rng);
            return std::make_pair(std::ref(combinedDistribution), std::ref(combinedDistribution));
        } else if (keyDistribution == Helpers::DESCENDING_STAR) {
            std::cout << "Generating distribution for descending*" << std::endl;
            //Special case, use one distribution for both
            Helpers::DistributionWrapper<DataType> combinedDistribution(Helpers::DESCENDING, minKey, maxKey, totalNumOperations, batchSize, rng);
            return std::make_pair(std::ref(combinedDistribution), std::ref(combinedDistribution));
        } else {
            std::cout << "Generating distribution for normal distribution" << std::endl;
            Helpers::DistributionWrapper<DataType> prefillDistribution(Helpers::UNIFORM, minKey, maxKey, numPrefill / batchSize + 1, batchSize, rng);
            Helpers::DistributionWrapper<DataType> opDistribution(keyDistribution, minKey, maxKey, numOperations, batchSize, rng);
            return std::make_pair(std::ref(prefillDistribution), std::ref(opDistribution));
        }
    }

    Helpers::DistributionWrapper<DataType> getKeyGen(Definitions::RandomEngine &rng) {
        return Helpers::DistributionWrapper<DataType>(keyDistribution, minKey, maxKey, numOperations, batchSize, rng);
    }

    Helpers::DistributionWrapper<DataType> getPrefillKeyGen(Definitions::RandomEngine &rng) {
        return Helpers::DistributionWrapper<DataType>(Helpers::UNIFORM, minKey, maxKey, numPrefill / batchSize + 1, batchSize, rng);
    }

    friend std::ostream& operator<<(std::ostream& out, const Configuration& config) {
        out << "iterations=" << config.iterations
            << " keyDistribution=" << KeyDistributionNames[config.keyDistribution]
            << " numPrefill=" << config.numPrefill
            << " numOperations=" << config.numOperations
            << " numThreads=" << config.numThreads
            << " insertWeight=" << config.insertWeight
            << " removeWeight=" << config.removeWeight
            << " searchWeight=" << config.searchWeight
            << " batchSize=" << config.batchSize
            << " seed=" << config.seed
            << " maxKey=" << config.maxKey
            << " cmakeConfiguration=" << getCmakePresetName() << " ";
        return out;
    }
};
}