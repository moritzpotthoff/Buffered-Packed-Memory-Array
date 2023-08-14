#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_get_random_seed.hpp"

#include <vector>
#include <algorithm>
#include <cstdint>

#include "batchedSparseArray.hpp"
#include "overallocatedBatchedSparseArray.hpp"
#include "simpleUnorderedSet.hpp"
#include "prefillGenerator.hpp"
#include "operationsGenerator.hpp"
#include "progress.hpp"
#include "definitions.hpp"
#include "profiler.hpp"
#include "randomDistributions.hpp"
#include "densityManager.hpp"

#include "tbb/scalable_allocator.h"

using RandomEngine = std::mt19937_64;

//Configuration for keyOnly tests:
using DataType = std::uint64_t;
struct KeyOf {
    static constexpr DataType keyOfValue(DataType value) {
        return value;
    }
};
using AllocatorType = tbb::scalable_allocator<DataType>;
using BatchDataStructure = DataStructures::BatchedSparseArray<DataType, DataType, 0, KeyOf, std::less<DataType>, AllocatorType, 128, 128, 64, DataStructures::DensityOne, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;
//using BatchDataStructure = DataStructures::OverallocatedBatchedSparseArray<DataType, DataType, 0, KeyOf, std::less<DataType>, AllocatorType, 128, 128, true, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;

using ReferenceSDS = DataStructures::SimpleUnorderedSet<DataType>;

//Configuration for complex tests (withMapped):
using KeyType = std::uint64_t;
using MappedType = int32_t;
using ValueType = std::pair<KeyType, MappedType>;
struct ComplexKeyOf {
    static constexpr DataType keyOfValue(ValueType value) {
        return value.first;
    }
};
using ComplexAllocatorType = tbb::scalable_allocator<ValueType>;
using ComplexBatchDataStructure = DataStructures::BatchedSparseArray<KeyType, ValueType, 0, ComplexKeyOf, std::less<KeyType>, ComplexAllocatorType, 128, 128, 64, DataStructures::DensityOne, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;
//using ComplexBatchDataStructure = DataStructures::OverallocatedBatchedSparseArray<KeyType, ValueType, 0, ComplexKeyOf, std::less<KeyType>, ComplexAllocatorType, 128, 128, true, Helpers::Profiler::SequentialProfiler, Helpers::NoDebugStream<std::thread::id>>;

const size_t defaultPrefillSmall = 5;
const size_t defaultOpsSmall = 50;
const size_t defaultPrefillLarge = 100;
const size_t defaultOpsLarge = 1000;
const std::int64_t defaultBatchSize = 1 << 15;
const std::uint64_t defaultNumberOfThreads = 64;

void validateSparseArray(BatchDataStructure &sds) {
    std::cout << "Validate batch sparse array" << std::endl;
    DataType currentHead = 0;
    DataType currentElement = 0;
    Helpers::Progress progress(sds.numberOfElements());
    for (int64_t i = 0; i < sds.size; ++i) {
        if (!sds.head(i) == 0) {
            REQUIRE(sds.head(i) > currentHead);
            currentHead = sds.head(i);
            AssertMsg(sds.head(i) == sds.pma[i].get(0), "Wrong head");
            REQUIRE(sds.head(i) == sds.pma[i].get(0));
            for (int64_t j = 0; j < sds.blockSize(i); ++j) {
                DataType element = sds.pma[i].get(j);
                AssertMsg(element > currentElement, "Not sorted");
                REQUIRE(element > currentElement);
                currentElement = element;
                ++progress;
            }
        }
    }
    progress.done();
}

/**
 * Tests if the data structure contains exactly the elements in values.
 * @param values
 */
void testContent(BatchDataStructure &sds, ReferenceSDS &reference) {
    validateSparseArray(sds);
    std::cout << "Test sds contains only valid values." << std::endl;
    Helpers::Progress progress(sds.numberOfElements());
    //Test that no other value is contained
    int j = 0;
    for (DataType element : sds) {
        REQUIRE(reference.search(element));
        ++progress;
        j++;
    }
    progress.done();

    //Test if all values are contained
    std::cout << "Test sds contains all values." << std::endl;
    progress.reset(reference.size());
    int i = 0;
    for (DataType val : reference) {
        if (!sds.contains(val)) {
            std::cout << "Element " << val << " from reference not contained in sds" << std::endl;
            std::cout << "Reference size is " << reference.size() << ", sds size is " << sds.numberOfElements() << std::endl;
            sds.printStats();
            AssertMsg(false, "check");
            REQUIRE(false);
        }
        ++progress;
        ++i;
    }
    progress.done();
    AssertMsg(sds.numberOfElements() == reference.numberOfElements(), "Wrong sizes!");
    REQUIRE(sds.numberOfElements() == reference.numberOfElements());
}

void runKeyOnlyTests(Helpers::KeyDistribution prefillDistribution, Helpers::KeyDistribution operationsDistribution, size_t numPrefillOps, size_t numOperations, std::int64_t batchSize, std::uint64_t numberOfThreads, std::uint64_t min, std::uint64_t max, uint32_t seed) {
    std::cout << "Seed is: " << seed << std::endl;
    //Create sds
    AllocatorType alloc;
    BatchDataStructure sds{alloc, numberOfThreads};
    ReferenceSDS reference;
#pragma omp parallel num_threads(numberOfThreads)
    {
        sds.profiler.registerThread();
    }

    //Create prefill
    Helpers::OperationsGenerator::BatchGenerator<DataType> prefillOps;
    RandomEngine rng;
    rng.seed(seed);
    Helpers::DistributionWrapper<DataType> prefillGen(prefillDistribution, min, max, numPrefillOps, batchSize, rng);
    prefillOps.generateOperations(numPrefillOps, 1.0, 0.0, 0.0, prefillGen, rng, batchSize);
    std::vector<Definitions::Operation<DataType>> prefill(prefillOps.ops);
    std::cout << "Running keyOnly prefill with distribution " << Helpers::KeyDistributionNames[prefillDistribution] << std::endl;
    Helpers::Progress progressPrefill(numPrefillOps);
    for (auto &operation : prefill) {
        AssertMsg(operation.type == Definitions::OperationType::BatchInsert, "Wrong prefill op");
        sds.insertBatch(operation.batch.start, operation.batch.size, numberOfThreads);
        reference.insertBatch(operation.batch.start, operation.batch.size);
        ++progressPrefill;
        //testContent(sds, reference);
    }
    progressPrefill.done();

    sds.profiler.activate();

    //Create values to insert
    Helpers::OperationsGenerator::BatchGenerator<DataType> gen;
    Helpers::DistributionWrapper<DataType> keyGen(operationsDistribution, min, max, numOperations, batchSize, rng);
    gen.generateOperations(numOperations, 0.7, 0.0, 0.3, keyGen, rng, batchSize);

    std::vector<Definitions::Operation<DataType>> ops(gen.ops);

    std::cout << "Running keyOnly operations with min=" << min << ", max=" << max << " and distribution " << Helpers::KeyDistributionNames[operationsDistribution] << "." << std::endl;
    Helpers::Progress progress(numOperations);
    for (auto &operation : ops) {
        switch (operation.type) {
            case Definitions::OperationType::Insert: {
                REQUIRE(false);
                break;
            }
            case Definitions::OperationType::Remove: {
                REQUIRE(false);
                break;
            }
            case Definitions::OperationType::Search: {
                bool success;
                asm volatile(""::"g"(&success));
                success = sds.contains(operation.value);
                asm volatile("":: : "memory");
                bool successReference = reference.search(operation.value);
                AssertMsg(success == successReference, "Search failed.");
                REQUIRE(success == successReference);
                break;
            }
            case Definitions::OperationType::BatchInsert: {
                sds.insertBatch(operation.batch.start, operation.batch.size, numberOfThreads);
                reference.insertBatch(operation.batch.start, operation.batch.size);
                //testContent(sds, reference);
                break;
            }
            case Definitions::OperationType::BatchRemove: {
                //sds.removeBatch(operation.batch.start, operation.batch.size);
                //reference.removeBatch(operation.batch.start, operation.batch.size);
                break;
            }
        }
        ++progress;
    }
    progress.done();

    sds.printStats();
    testContent(sds, reference);
    sds.profiler.print();
}

void testComplexContent(ComplexBatchDataStructure &sds, std::map<KeyType, MappedType> &reference) {
    std::cout << "Test sds contains only valid values." << std::endl;
    Helpers::Progress progress(sds.numberOfElements());
    // Test that no other value is contained
    int j = 0;
    for (ValueType element : sds) {
        REQUIRE(reference.contains(element.first));
        REQUIRE(reference[element.first] == element.second);//Test that the value is actually the same, too.
        ++progress;
        j++;
    }
    progress.done();

    //Test if all values are contained
    std::cout << "Test sds contains all values." << std::endl;
    progress.reset(reference.size());
    int i = 0;
    for (ValueType val : reference) {
        REQUIRE(reference.contains(val.first));//Test contains
        auto it = reference.find(val.first);//Test iterator returned by find()
        REQUIRE((*it).first == val.first);
        REQUIRE((*it).second == val.second);
        ++progress;
        ++i;
    }
    progress.done();
    REQUIRE(sds.numberOfElements() == reference.size());
}

void runWithMappedTests(Helpers::KeyDistribution prefillDistribution, Helpers::KeyDistribution operationsDistribution, size_t numPrefillOps, size_t numOperations, std::int64_t batchSize, std::uint64_t numberOfThreads, std::uint64_t min, std::uint64_t max, uint32_t seed) {
    std::cout << "Seed is: " << seed << std::endl;
    ComplexAllocatorType alloc;
    ComplexBatchDataStructure sds{alloc, numberOfThreads};
    std::map<KeyType, MappedType> reference;

    //Create prefill
    std::cout << "Creating prefill" << std::endl;
    Helpers::OperationsGenerator::ComplexBatchGenerator<KeyType, MappedType, ValueType> prefillGen;
    RandomEngine rng;
    rng.seed(seed);
    Helpers::DistributionWrapper<KeyType> prefillDistr(prefillDistribution, min, max, numPrefillOps, batchSize, rng);
    Helpers::DistributionWrapper<MappedType> prefillMappedDistr(Helpers::KeyDistribution::UNIFORM, min, max, numPrefillOps, batchSize, rng);
    prefillGen.generateOperations(numPrefillOps, 1.0, 0.0, 0.0, prefillDistr, prefillMappedDistr, rng, batchSize);
    std::vector<Definitions::Operation<KeyType, ValueType>> prefill(prefillGen.ops);
    std::cout << "Running withMapped prefill with distribution " << Helpers::KeyDistributionNames[prefillDistribution] << std::endl;
    Helpers::Progress progressPrefill(numPrefillOps);
    for (auto &operation : prefill) {
        AssertMsg(operation.type == Definitions::OperationType::BatchInsert, "Wrong prefill op");
        sds.insertBatch(operation.batch.start, operation.batch.size, numberOfThreads);
        for (int i = 0; i < operation.batch.size; ++i) {
            reference.insert(operation.batch.start[i]);
        }
        ++progressPrefill;
    }
    progressPrefill.done();

    Helpers::OperationsGenerator::ComplexBatchGenerator<KeyType, MappedType, ValueType> gen;
    Helpers::DistributionWrapper<KeyType> keyGen(operationsDistribution, min, max, numOperations, batchSize, rng);
    Helpers::DistributionWrapper<MappedType> mappedGen(Helpers::KeyDistribution::UNIFORM, min, max, numOperations, batchSize, rng);
    gen.generateOperations(numOperations, 0.7, 0.0, 0.3, keyGen, mappedGen, rng, batchSize);

#pragma omp parallel num_threads(numberOfThreads)
    {
        sds.profiler.registerThread();
    }
    sds.profiler.activate();

    std::vector<Definitions::Operation<KeyType, ValueType>> ops(gen.ops);

    std::cout << "Running withMapped operations with min=" << min << ", max=" << max << " and distribution " << Helpers::KeyDistributionNames[operationsDistribution] << "." << std::endl;
    Helpers::Progress progress(numOperations);
    for (auto &operation : ops) {
        switch (operation.type) {
            case Definitions::OperationType::Insert: {
                REQUIRE(false);
                break;
            }
            case Definitions::OperationType::Remove: {
                REQUIRE(false);
                break;
            }
            case Definitions::OperationType::Search: {
                bool success;
                asm volatile(""::"g"(&success));
                success = sds.contains(operation.value);
                asm volatile("":: : "memory");
                bool successReference = reference.contains(operation.value);
                AssertMsg(success == successReference, "Search failed.");
                REQUIRE(success == successReference);
                auto it = sds.find(operation.value);
                if (successReference) {
                    //Contained, check mapped value
                    REQUIRE((*it).first == operation.value);
                    AssertMsg((*it).second == reference[operation.value], "Wrong value from iterator");
                    REQUIRE((*it).second == reference[operation.value]);
                } else {
                    //Not contained, check that iterator points to end
                    REQUIRE(it == sds.end());
                }
                break;
            }
            case Definitions::OperationType::BatchInsert: {
                sds.insertBatch(operation.batch.start, operation.batch.size, numberOfThreads);
                for (int i = 0; i < operation.batch.size; ++i) {
                    reference.insert(operation.batch.start[i]);
                }
                break;
            }
            case Definitions::OperationType::BatchRemove: {
                REQUIRE(false);
                break;
            }
        }
        ++progress;
    }
    progress.done();
    REQUIRE(sds.isValid());
    sds.printStats();
    testComplexContent(sds, reference);
    sds.profiler.print();
}

//Key only small tests:

TEST_CASE("KeyOnly small test dense normal with no duplicates", "[sds][keyOnly][small][denseNormal][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DENSE_NORMAL;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test uniform with no duplicates", "[sds][keyOnly][small][uniform][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test uniform with duplicates", "[sds][keyOnly][small][uniform][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test zipf with no duplicates", "[sds][keyOnly][small][zipf][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ZIPF;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test zipf with duplicates", "[sds][keyOnly][small][zipf][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ZIPF;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test ascending with no duplicates", "[sds][keyOnly][small][ascending][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test ascending with duplicates", "[sds][keyOnly][small][ascending][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test ascending with no duplicates without prefill", "[sds][keyOnly][small][ascending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test ascending with duplicates without prefill", "[sds][keyOnly][small][ascending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test descending with no duplicates", "[sds][keyOnly][small][descending][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test descending with duplicates", "[sds][keyOnly][small][descending][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test descending with no duplicates without prefill", "[sds][keyOnly][small][descending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test descending with duplicates without prefill", "[sds][keyOnly][small][descending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

//Tests for small batches

TEST_CASE("KeyOnly small test dense normal with no duplicates small batch", "[smallBatch][sds][keyOnly][small][denseNormal][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DENSE_NORMAL;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test uniform with no duplicates small batch", "[smallBatch][sds][keyOnly][small][uniform][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test descending with no duplicates small batch", "[smallBatch][sds][keyOnly][small][descending][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test descending with duplicates small batch", "[smallBatch][sds][keyOnly][small][descending][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test descending with no duplicates without prefill small batch", "[smallBatch][sds][keyOnly][small][descending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly small test descending with duplicates without prefill small batch", "[smallBatch][sds][keyOnly][small][descending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}


//Small tests with mapped values:

TEST_CASE("WithMapped small test uniform with no duplicates", "[sds][withMapped][small][uniform][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test uniform with duplicates", "[sds][withMapped][small][uniform][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test zipf with no duplicates", "[sds][withMapped][small][zipf][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ZIPF;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test zipf with duplicates", "[sds][withMapped][small][zipf][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ZIPF;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test ascending with no duplicates", "[sds][withMapped][small][ascending][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test ascending with duplicates", "[sds][withMapped][small][ascending][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test ascending with no duplicates without prefill", "[sds][withMapped][small][ascending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test ascending with duplicates without prefill", "[sds][withMapped][small][ascending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test descending with no duplicates", "[sds][withMapped][small][descending][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test descending with duplicates", "[sds][withMapped][small][descending][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillSmall;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test descending with no duplicates without prefill", "[sds][withMapped][small][descending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped small test descending with duplicates without prefill", "[sds][withMapped][small][descending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsSmall;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}






//Key only large tests:

TEST_CASE("KeyOnly large test uniform with no duplicates", "[sds][keyOnly][large][uniform][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test uniform with duplicates", "[sds][keyOnly][large][uniform][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test zipf with no duplicates", "[sds][keyOnly][large][zipf][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ZIPF;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test zipf with duplicates", "[sds][keyOnly][large][zipf][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ZIPF;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test ascending with no duplicates", "[sds][keyOnly][large][ascending][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test ascending with duplicates", "[sds][keyOnly][large][ascending][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test ascending with no duplicates without prefill", "[sds][keyOnly][large][ascending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test ascending with duplicates without prefill", "[sds][keyOnly][large][ascending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test descending with no duplicates", "[sds][keyOnly][large][descending][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test descending with duplicates", "[sds][keyOnly][large][descending][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test descending with no duplicates without prefill", "[sds][keyOnly][large][descending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test descending with duplicates without prefill", "[sds][keyOnly][large][descending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test uniform with no duplicates small batch", "[smallBatch][sds][keyOnly][large][uniform][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test uniform with duplicates small batch", "[smallBatch][sds][keyOnly][large][uniform][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test descending with no duplicates without prefill small batch", "[smallBatch][sds][keyOnly][large][descending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("KeyOnly large test descending with duplicates without prefill small batch", "[smallBatch][sds][keyOnly][large][descending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = 100;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runKeyOnlyTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

//Tests with mapped values:

TEST_CASE("WithMapped large test uniform with no duplicates", "[sds][withMapped][large][uniform][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test uniform with duplicates", "[sds][withMapped][large][uniform][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::UNIFORM;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test zipf with no duplicates", "[sds][withMapped][large][zipf][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ZIPF;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test zipf with duplicates", "[sds][withMapped][large][zipf][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ZIPF;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test ascending with no duplicates", "[sds][withMapped][large][ascending][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test ascending with duplicates", "[sds][withMapped][large][ascending][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test ascending with no duplicates without prefill", "[sds][withMapped][large][ascending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test ascending with duplicates without prefill", "[sds][withMapped][large][ascending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::ASCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test descending with no duplicates", "[sds][withMapped][large][descending][noDuplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test descending with duplicates", "[sds][withMapped][large][descending][duplicates]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = defaultPrefillLarge;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test descending with no duplicates without prefill", "[sds][withMapped][large][descending][noDuplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 1000000000000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}

TEST_CASE("WithMapped large test descending with duplicates without prefill", "[sds][withMapped][large][descending][duplicates][noPrefill]") {
    const uint32_t seed = Catch::getSeed();
    const size_t numberOfPrefillOperations = 0;
    const size_t numberOfOperations = defaultOpsLarge;
    const std::int64_t batchSize = defaultBatchSize;
    const std::uint64_t numberOfThreads = defaultNumberOfThreads;
    const std::uint64_t min = 1;
    const std::uint64_t max = 500000;
    const Helpers::KeyDistribution prefillDistribution = Helpers::KeyDistribution::UNIFORM;
    const Helpers::KeyDistribution operationsDistribution = Helpers::KeyDistribution::DESCENDING;

    runWithMappedTests(prefillDistribution, operationsDistribution, numberOfPrefillOperations, numberOfOperations, batchSize, numberOfThreads, min, max, seed);
}







TEST_CASE("Ascending distribution test", "[distr][asc]") {
    const size_t numberOfOperations = 1 << 9;
    const std::int64_t batchSize = 1 << 15;
    const std::uint64_t min = 1;
    const std::uint64_t max = 10000000000000;

    const uint32_t seed = Catch::getSeed();
    Helpers::OperationsGenerator::BatchGenerator<DataType> gen;
    RandomEngine rng;
    rng.seed(seed);
    Helpers::DistributionWrapper<DataType> keyGen(Helpers::KeyDistribution::ASCENDING, min, max, numberOfOperations, batchSize, rng);
    gen.generateOperations(numberOfOperations, 0.7, 0.0, 0.3, keyGen, rng, batchSize);
    std::vector<Definitions::Operation<DataType>> ops(gen.ops);

    std::cout << "Testing the generated values" << std::endl;
    DataType lastValue = 0;
    Helpers::Progress progress(numberOfOperations);
    for (auto &operation : ops) {
        switch (operation.type) {
            case Definitions::OperationType::Insert: {
                REQUIRE(false);
                break;
            }
            case Definitions::OperationType::Remove: {
                REQUIRE(false);
                break;
            }
            case Definitions::OperationType::Search: {
                REQUIRE(operation.value >= lastValue);
                lastValue = operation.value;
                break;
            }
            case Definitions::OperationType::BatchInsert: {
                for (int i = 0; i < operation.batch.size; ++i) {
                    REQUIRE(operation.batch.start[i] >= lastValue);
                    lastValue = operation.batch.start[i];
                }
                break;
            }
            case Definitions::OperationType::BatchRemove: {
                for (int i = 0; i < operation.batch.size; ++i) {
                    REQUIRE(operation.batch.start[i] >= lastValue);
                    lastValue = operation.batch.start[i];
                }
                break;
            }
        }
        ++progress;
    }
    progress.done();
}


TEST_CASE("Descending distribution test", "[distr][desc]") {
    const size_t numberOfOperations = 1 << 9;
    const std::int64_t batchSize = 1 << 15;
    const std::uint64_t min = 1;
    const std::uint64_t max = 10000000000000;

    const uint32_t seed = Catch::getSeed();
    Helpers::OperationsGenerator::BatchGenerator<DataType> gen;
    RandomEngine rng;
    rng.seed(seed);
    Helpers::DistributionWrapper<DataType> keyGen(Helpers::KeyDistribution::DESCENDING, min, max, numberOfOperations, batchSize, rng);
    gen.generateOperations(numberOfOperations, 0.7, 0.0, 0.3, keyGen, rng, batchSize);
    std::vector<Definitions::Operation<DataType>> ops(gen.ops);

    std::cout << "Testing the generated values" << std::endl;
    DataType lastValue = max;
    Helpers::Progress progress(numberOfOperations);
    for (auto &operation : ops) {
        switch (operation.type) {
            case Definitions::OperationType::Insert: {
                REQUIRE(false);
                break;
            }
            case Definitions::OperationType::Remove: {
                REQUIRE(false);
                break;
            }
            case Definitions::OperationType::Search: {
                REQUIRE(operation.value <= lastValue);
                lastValue = operation.value;
                break;
            }
            case Definitions::OperationType::BatchInsert: {
                //Test that batch is sorted ascending
                DataType batchValue = operation.batch.start[0];
                for (int i = 0; i < operation.batch.size; ++i) {
                    REQUIRE(operation.batch.start[i] >= batchValue);
                    REQUIRE(operation.batch.start[i] <= lastValue);
                    batchValue = operation.batch.start[i];
                }
                lastValue = operation.batch.start[0];
                break;
            }
            case Definitions::OperationType::BatchRemove: {
                for (int i = 0; i < operation.batch.size; ++i) {
                    REQUIRE(operation.batch.start[i] <= lastValue);
                    lastValue = operation.batch.start[i];
                }
                break;
            }
        }
        ++progress;
    }
    progress.done();
}

struct RangeQuery {
    DataType start;
    DataType end;
};

TEST_CASE("Range query test", "[range]") {
    const uint32_t seed = Catch::getSeed();
    RandomEngine rng;
    size_t numberOfRangeQueries = 1000;
    size_t min = 1;
    size_t max = 10000000000;
    size_t numPrefillOps = 500;
    size_t batchSize = 10000;

    AllocatorType alloc;
    BatchDataStructure sds{alloc, defaultNumberOfThreads};
    ReferenceSDS reference;
#pragma omp parallel num_threads(defaultNumberOfThreads)
    {
    sds.profiler.registerThread();
    }

    //Create prefill
    Helpers::OperationsGenerator::BatchGenerator<DataType> prefillOps;
    rng.seed(seed);

    Helpers::DistributionWrapper<DataType> prefillGen(Helpers::KeyDistribution::UNIFORM, min, max, numPrefillOps, batchSize, rng);
    prefillOps.generateOperations(numPrefillOps, 1.0, 0.0, 0.0, prefillGen, rng, batchSize);
    std::vector<Definitions::Operation<DataType>> prefill(prefillOps.ops);
    std::cout << "Inserting the elements" << std::endl;
    Helpers::Progress progress(numPrefillOps);
    for (auto &operation : prefill) {
        AssertMsg(operation.type == Definitions::OperationType::BatchInsert, "Wrong prefill op");
        sds.insertBatch(operation.batch.start, operation.batch.size, defaultNumberOfThreads);
        reference.insertBatch(operation.batch.start, operation.batch.size);
        ++progress;
    }
    progress.done();

    //Prepare the queries
    std::cout << "Preparing the queries" << std::endl;
    progress.reset(numberOfRangeQueries);
    Helpers::UniformDistribution startDistribution(min, max);
    Helpers::UniformDistribution keyDifferenceDistribution(min, max);
    std::vector<RangeQuery> queries(numberOfRangeQueries);
    for (RangeQuery &query : queries) {
        query.start = startDistribution(rng);
        query.end = query.start + keyDifferenceDistribution(rng);
        ++progress;
    }
    progress.done();
    sds.printStats();
    testContent(sds, reference);

    //Run the queries
    std::cout << "Running the queries" << std::endl;
    progress.reset(numberOfRangeQueries);
    for (const RangeQuery &query : queries) {
        size_t queryLength = 0;
        size_t querySum = 0;
        sds.rangeQuery(query.start, query.end, [&queryLength, &querySum](const DataType &key) {
            ++queryLength;
            querySum += key;
        });

        size_t referenceQueryLength = 0;
        size_t referenceQuerySum = 0;
        reference.rangeQuery(query.start, query.end, [&referenceQueryLength, &referenceQuerySum](const DataType &key) {
            ++referenceQueryLength;
            referenceQuerySum += key;
        });

        //std::cout << "Query " << query.start << " -- " << query.end << " with result " << querySum << " from " << queryLength << " entries" << std::endl;

        AssertMsg(referenceQueryLength == queryLength, "Wrong number of elements queried.");
        AssertMsg(referenceQuerySum == querySum, "Wrong sum in range query.");
        REQUIRE(referenceQueryLength == queryLength);
        REQUIRE(referenceQuerySum == querySum);
        ++progress;
    }
    progress.done();
}
