#pragma once

#include <iostream>
#include <memory>
#include <omp.h>
#include <atomic>

#include "helpers.hpp"
#include "assert.hpp"
#include "profiler.hpp"
#include "debugStream.hpp"
#include "block.hpp"
#include "tbb/concurrent_map.h"
#include "binaryRebalancingManager.hpp"
#include "densityManager.hpp"
#include "typedefs.hpp"
#include "iterators.hpp"
#include "concurrentBucketMap.hpp"

#define EXTENDED_TESTS false
#define USE_BUCKET_MAP true

namespace DataStructures {
template<typename KEY, typename VALUE, KEY NULL_KEY, typename KEY_OF, typename KEY_COMPARE = std::less<KEY>, typename ALLOCATOR = std::allocator<VALUE>, UnsignedSizeType MAX_NUMBER_OF_THREADS = 128, BlockSizeType BLOCK_SIZE = 128, BlockSizeType SEGMENT_WIDTH = 2048, typename DENSITY_CONTROLS = DensityThree, typename PROFILER = Helpers::Profiler::NoProfiler, typename DEBUG_STREAM = Helpers::NoDebugStream<KEY>>
class BatchedSparseArray {
private:
    //Size configuration
    static constexpr SizeType LogInitialArrayCapacity = 12;
    static constexpr SizeType InitialArrayCapacity = 1LL << LogInitialArrayCapacity;//The initial capacity (in blocks). Must be a power of 2.
    static constexpr BlockSizeType BlockSize = BLOCK_SIZE;//Number of elements in each block, must be divisible by 2.
    static constexpr BlockSizeType SegmentWidth = SEGMENT_WIDTH;//Number of elements in each segment, must be divisible by 2.
    static constexpr BlockSizeType MinBlockSize = BlockSize / 2;//Minimum number of elements that each block has, unless there is only one block. Must not be greater than BlockSize / 2.

    using DensityManager = DENSITY_CONTROLS;

    //Configuration of the operations
    static constexpr UnsignedSizeType MaxNumberOfThreads = MAX_NUMBER_OF_THREADS;
    static constexpr SizeType ParallelPrefixSumMinimum = 8 * MaxNumberOfThreads;
    static constexpr SizeType BinarySearchCutoff = 100;//Minimum size of an interval such that binary search is used. Below, use linear search.
    static constexpr double InitialOverallocationFactor = 1.8;//Factor of extra width that is used during a global rewriting operation.

    //Debugging configuration
    using DebugStream = DEBUG_STREAM;
    using Profiler = PROFILER;
    static constexpr bool Debug = false;//Default: false
    static constexpr bool Profile = std::is_same_v<PROFILER, Helpers::Profiler::SequentialProfiler>;
    static constexpr bool ProfileConcurrent = std::is_same_v<PROFILER, Helpers::Profiler::ConcurrentProfiler>;

public:
    //ValueType is the type of objects stored in the blocks.
    //KeyType is the type that is used for comparison. It is stored in the packed memory array.
    //KeyOf offers a KeyOf::keyOfValue-function that extracts the key from a value.
    //KeyCompare offers an access operator (left, right), that compares two keys.
    using ValueType = VALUE;
    using Reference = ValueType&;
    using Pointer = ValueType*;
    using ConstReference = ValueType const&;
    using KeyType = KEY;
    using KeyCompare = KEY_COMPARE;
    using KeyOf = KEY_OF;
    using Allocator = ALLOCATOR;
    using AllocatorTraits = std::allocator_traits<Allocator>;
    using SdsType = BatchedSparseArray<KeyType, ValueType, NULL_KEY, KeyOf, KeyCompare, Allocator, MaxNumberOfThreads, BlockSize, SegmentWidth, DensityManager, Profiler, DebugStream>;
    using iterator = BlockSparseArrayIterator<SdsType>;

    //Operation configuration
    static constexpr bool UseInverseMerge = true;//Default: true. If true, will distribute the work of merged tasks to the tasks that are involved. Otherwise, one task does all the work.
    static constexpr bool SplitRebalancingTasks = true;

private:
    //using RebalancingManager = DataStructures::RebalancingManager<SdsType, DensityManager, Allocator>;
    using RebalancingManager = DataStructures::BinaryRebalancingManager<SegmentWidth, SdsType, DensityManager, Allocator>;
    using PmaEntryType = PmaEntry<KeyType, ValueType, NULL_KEY, KeyOf, KeyCompare, Allocator, MinBlockSize, BlockSize, SdsType>;
    using PmaEntryReference = PmaEntryType&;
    using PmaEntryPointer = PmaEntryType* ;
    using AllocatorRebalancingNodes = tbb::scalable_allocator<RebalancingNode>;
    using RebalancingNodeContainer = oneapi::tbb::concurrent_vector<RebalancingNode, AllocatorRebalancingNodes>;
    using AllocatorConstRecountingNodes = tbb::scalable_allocator<std::pair<const IndexType, bool>>;
#if USE_BUCKET_MAP
    using RecountingNodeMap = ConcurrentBucketMap;
#else
    using RecountingNodeMap = oneapi::tbb::concurrent_map<IndexType, bool, std::less<IndexType>, AllocatorConstRecountingNodes>;
#endif

    friend class BlockSparseArrayIterator<SdsType>;
    friend class DataStructures::BinaryRebalancingManager<SegmentWidth, SdsType, DensityManager, Allocator>;

public:
    SizeType capacity;//The capacity of the pma
    SizeType swapCapacity;//The capacity of the swap pma
    SizeType bufferCapacity;//The capacity of the buffer array
    SizeType size;//End of the region of entries that might be used
    [[no_unique_address]] KeyCompare comp;
    //The main packed memory array
    std::unique_ptr<PmaEntryType[]> pma;
    //A pma that is used as scratchpad during out-of-place rewrites
    std::unique_ptr<PmaEntryType[]> swapPma;
    //Buffer array that stores new blocks between their creation and insertion and the merge with the pma in rebalancing
    std::unique_ptr<PmaEntryType[]> bufferArray;
    //Pointers from pma blocks to buffer array entries.
    std::unique_ptr<Interval[]> bufferPointers;
    RebalancingManager rebalancingManager;
    RebalancingNodeContainer rebalancingNodes;
    RecountingNodeMap recountingNodesMap;
#if !USE_BUCKET_MAP
    std::vector<std::pair<IndexType, bool>> recountingNodesVector;
#endif
    std::vector<RebalancingTask> rebalancingTasks;
    //Prefix sum over the number of blocks to rewrite in the rebalancing tasks
    std::vector<SizeType> prefixNumberOfRebalancingBlocks;
    alignas(64) std::array<TaskInfo, MaxNumberOfThreads + 1> tasks;// + 1 as sentinel
    [[no_unique_address]] Allocator allocator;
    UnsignedIndexType numberOfThreads;
    //For debugging and profiling.
    [[no_unique_address]] Profiler profiler;

    [[no_unique_address]] DebugStream debugStream;

#ifndef NDEBUG
    std::atomic_int64_t bufferWrites;
    std::atomic_int64_t bufferReads;
#endif

public:
    BatchedSparseArray(Allocator &alloc, UnsignedIndexType numThreads) :
        capacity(InitialArrayCapacity),
        swapCapacity(InitialArrayCapacity),
        bufferCapacity(InitialArrayCapacity),
        size(0),
        comp(),
        rebalancingManager(this, numThreads),
        rebalancingNodes(0),
#if USE_BUCKET_MAP
        recountingNodesMap(numThreads),
#endif
        rebalancingTasks(0),
        prefixNumberOfRebalancingBlocks(0),
        allocator(alloc),
        numberOfThreads(numThreads) {
        //Allocate the necessary space and initialize it to all zero.
        pma = std::make_unique<PmaEntryType[]>(capacity);
        swapPma = std::make_unique<PmaEntryType[]>(swapCapacity);
        bufferArray = std::make_unique<PmaEntryType[]>(bufferCapacity);
        bufferPointers = std::make_unique<Interval[]>(capacity);
    }

    ~BatchedSparseArray() {
        for (IndexType i = 0; i < size; ++i) {
            if (!pma[i].isEmpty()) {
                pma[i].deleteBlock(allocator);
            }
        }
    }

public:
    void insertBatch(Pointer batch, SizeType batchSize, [[maybe_unused]] UnsignedIndexType numThreads) {
        AssertMsg(numThreads == numberOfThreads, "Wrong number of threads!");
#ifndef NDEBUG
        bufferWrites = bufferReads = 0;
#endif
        AssertMsg(batchSorted(batch, batchSize), "Batch not sorted.");
        AssertMsg(numberOfThreads <= MaxNumberOfThreads, "Too many threads.");
        AssertMsg(batchSize >= static_cast<SizeType>(numberOfThreads), "Batch is too small.");
        if constexpr (Profile) profiler.startOperation(Helpers::Profiler::Operation::INSERT);
        if constexpr (Debug) std::cout << "Distributing a batch of size " << batchSize << " with " << numberOfThreads << " tasks." << std::endl;
        //A: Insertion Phase
        //A-1. Find the block corresponding to the splitter using binary search and compute the required widths.
        if constexpr (Profile) profiler.startPhase(Helpers::Profiler::Phase::INSERTION_DISTRIBUTION);
        const SizeType splitterDistance = batchSize / static_cast<SizeType>(numberOfThreads);
        //Compute the last task info as sentinel
        tasks[numberOfThreads].blockForBatchSplitter = size;//sentinel, as above for the end
        tasks[numberOfThreads].firstBatchElement = batchSize;//Sentinel for the last task
        //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) shared(tasks) firstprivate(size, batchSize, splitterDistance, batch)
        {
            const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
            tasks[thread].blockForBatchSplitter = findBlockIndex(KeyOf::keyOfValue(batch[thread * static_cast<UnsignedIndexType>(splitterDistance)]));
            if (thread > 0) {
                tasks[thread].firstBatchElement = findFirstBatchElement(batch, static_cast<IndexType>(thread) * splitterDistance, head(tasks[thread].blockForBatchSplitter));
            } else {
                //Thread 0
                tasks[thread].firstBatchElement = 0;
            }
            IndexType nextBlockForBatchSplitter, nextFirstBatchElement;
            if (thread == numberOfThreads - 1) {
                nextBlockForBatchSplitter = size;
                nextFirstBatchElement = batchSize;
            } else {
                nextBlockForBatchSplitter = findBlockIndex(KeyOf::keyOfValue(batch[(thread + 1) * static_cast<UnsignedIndexType>(splitterDistance)]));
                nextFirstBatchElement = findFirstBatchElement(batch, static_cast<IndexType>(thread + 1) * splitterDistance, head(nextBlockForBatchSplitter));
            }

            tasks[thread].requiredWidth = getRequiredWidth(nextFirstBatchElement - tasks[thread].firstBatchElement);//Number of batch elements in this task
        }

        //A-2. Sequentially fix the first element in the batch if necessary. This avoids more than one tasks working on a single block.
        // Also accumulate the required widths. Must not be parallelized!
        tasks[0].firstBatchElement = 0;
        tasks[0].type = TaskType::INSERTION;//default
        tasks[0].mergedTasks = 0;
        SizeType mergedTasks = 0;
        bool hasMergedTask = false;
        for (UnsignedIndexType thread = 1; thread < numberOfThreads; ++thread) {
            if (tasks[thread].blockForBatchSplitter == tasks[thread - 1].blockForBatchSplitter) {
                //If (at least) two tasks start at the first block, make sure the last of those task does all the work for this block
                tasks[thread].firstBatchElement = tasks[thread - 1].firstBatchElement;
                tasks[thread].type = TaskType::INSERTION;
                if constexpr (UseInverseMerge) {
                    tasks[thread - 1].type = TaskType::INVERSE_MERGE;
                    hasMergedTask = true;
                    ++mergedTasks;
                    tasks[thread].mergedTasks = mergedTasks;
                    //std::cout << "Task " << (thread - 1) << " is inverse merge task, waits distribution. Merged tasks  = " << mergedTasks << std::endl;
                } else {
                    tasks[thread].requiredWidth += tasks[thread - 1].requiredWidth;
                    tasks[thread].mergedTasks = 0;
                }
                tasks[thread - 1].requiredWidth = 0;
            } else {
                if constexpr (UseInverseMerge) {
                    tasks[thread].type = TaskType::INSERTION;
                    tasks[thread].mergedTasks = 0;
                    mergedTasks = 0;
                }
            }
        }
        tasks[numberOfThreads].requiredWidth = 0;//Sentinel

        //A-3. Distribute the work if multiple threads share the same block
        if constexpr (UseInverseMerge) {
            distributeWorkInMergedTasks(batch, batchSize);
        }

        //A-4. Compute prefix sum over task widths to get start indices in buffer array
        SizeType prefixWidth = tasks[0].blockForBatchSplitter;//Start writing only after the blocks that are not touched
        for (UnsignedIndexType thread = 0; thread <= numberOfThreads; ++thread) {
            tasks[thread].startIndex = prefixWidth;
            prefixWidth += tasks[thread].requiredWidth;
            if constexpr (Debug) std::cout << "   Task " << thread << ": " << tasks[thread] << std::endl;
            if (thread < numberOfThreads && tasks[thread].type == TaskType::INSERTION && tasks[thread].mergedTasks > 0) {
                AssertMsg(thread >= tasks[thread].mergedTasks, "Not sufficient threads for merge group size.");
                //Set the buffer array pointers of merge block to contain any blocks that might be written during execution.
                const IndexType mergeBlock = tasks[thread - 1].blockForBatchSplitter;
                bufferPointers[mergeBlock].left = tasks[thread - tasks[thread].mergedTasks].startIndex;
                bufferPointers[mergeBlock].right = tasks[thread].startIndex;
                AssertMsg(bufferPointers[mergeBlock].left >= 0 && bufferPointers[mergeBlock].right >= bufferPointers[mergeBlock].left, "Invalid buffer range");
            }
        }

        //A-5. Enlarge the buffer array as needed
        increaseBufferArraySize(prefixWidth);
        if constexpr (Profile) profiler.endPhase(Helpers::Profiler::Phase::INSERTION_DISTRIBUTION);

        //A-6. Start the insertions in the respective intervals
        if constexpr (Profile) profiler.startPhase(Helpers::Profiler::Phase::INSERTION_EXECUTION);
        //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) firstprivate(batch, batchSize)
        {
            const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
            if constexpr (Debug) debugStream.add("Thread starting task " + std::to_string(thread));
            startInsertionTask(thread, batch, batchSize);
        }

        //A-7. Postprocess after inverse merge.
        if constexpr (UseInverseMerge) {
            if (hasMergedTask) {
                //Remove the merge blocks of inverse merge tasks
                postprocessInverseMergeTasks();

                //Compacitfy the buffer blocks of the merge group
                compactifyBufferBlocks();
            }
        }
        if constexpr (Profile) profiler.endPhase(Helpers::Profiler::Phase::INSERTION_EXECUTION);

#if !defined NDEBUG && EXTENDED_TESTS
        isValid();
#endif

        //B: Rebalancing Phase
        //B-1 Collect the recounting nodes from the map into the vector.
        if constexpr (Profile) profiler.startPhase(Helpers::Profiler::Phase::UPDATE);
#if !USE_BUCKET_MAP
        copyRecountingNodesToVector(recountingNodesMap);
        recountingNodesMap.clear();
#endif

        //B-2: Update the hierarchy using the global recounting nodes and get the rebalancingNodes that must be executed.
        //Profiling will be done inside the update() method.
        rebalancingManager.update();
#if !USE_BUCKET_MAP
        recountingNodesVector.clear();
#endif

        //B-3: Create rebalancing tasks and remove duplicate rebalancing tasks.
        SizeType numberOfRebalancingTasks = 0;
        rebalancingTasks.resize(rebalancingNodes.size());
        //std::cout << "Number of rebalancing nodes after update: " << rebalancingNodes.size() << std::endl;
        for (IndexType i = 0; i < static_cast<IndexType>(rebalancingNodes.size()); ++i) {
            const bool dominated = rebalancingManager.rebalancingNodeDominated(rebalancingNodes[i]);
            if (!dominated) {
                rebalancingTasks[numberOfRebalancingTasks] = rebalancingManager.getRebalancingTask(rebalancingNodes[i]);
                ++numberOfRebalancingTasks;
            } else {
                rebalancingManager.clearRebalancedFlag(rebalancingNodes[i]);
            }
        }
        rebalancingNodes.clear();
        rebalancingTasks.resize(numberOfRebalancingTasks);
        if constexpr (Profile) profiler.endPhase(Helpers::Profiler::Phase::UPDATE);


        if constexpr (Profile) profiler.startPhase(Helpers::Profiler::Phase::REBALANCING_DISTRIBUTION);
        //std::cout << "Number of rebalancing tasks is " << numberOfRebalancingTasks << std::endl;
        if (numberOfRebalancingTasks == 0) {
            //Nothing to rebalance, finish the operation!
            //std::cout << "   Finishing the operation" << std::endl;
            rebalancingTasks.clear();
            prefixNumberOfRebalancingBlocks.clear();

            if constexpr (Profile) profiler.endPhase(Helpers::Profiler::Phase::REBALANCING_DISTRIBUTION);
            if constexpr (Profile) profiler.endOperation(Helpers::Profiler::Operation::INSERT);
#if !defined NDEBUG && EXTENDED_TESTS
            isValid();
#endif
            return;
        }
        //Calculate the inclusive prefix sum over the number of blocks in each rebalancing task.
        prefixNumberOfRebalancingBlocks.resize(numberOfRebalancingTasks + 1);
        std::inclusive_scan(rebalancingTasks.begin(), rebalancingTasks.end(), prefixNumberOfRebalancingBlocks.begin(), [](SizeType sum, const RebalancingTask &task) {
            return sum + task.numberOfBlocks;
        }, static_cast<SizeType>(0));
        const SizeType numberOfBlocksToRebalance = prefixNumberOfRebalancingBlocks[numberOfRebalancingTasks - 1];
        //Sentinel
        prefixNumberOfRebalancingBlocks[numberOfRebalancingTasks] = prefixNumberOfRebalancingBlocks[numberOfRebalancingTasks - 1];


#if !defined NDEBUG && EXTENDED_TESTS
        isValid();
        rebalancingManager.validate(size + 1);

        for (IndexType i = 0; i < numberOfRebalancingTasks; ++i) {
            for (IndexType j = 0; j < numberOfRebalancingTasks; ++j) {
                if (i == j) continue;
                AssertMsg(rebalancingTasks[i].rootNode != rebalancingTasks[j].rootNode, "Duplicate rebalancing task!");
                AssertMsg(!rebalancingTasks[i].pmaRange.overlaps(rebalancingTasks[j].pmaRange), "Overlapping rebalancing task!");
                AssertMsg(!rebalancingTasks[j].pmaRange.overlaps(rebalancingTasks[i].pmaRange), "Overlapping rebalancing task!");
            }
        }
#endif

        //B-4. Compute the new size and resize the swapPma.
        SizeType newPmaSize = size;
        if (rebalancingTasks.size() > 0) {
            const IndexType lastRebalancingTaskEnd = rebalancingTasks.back().pmaRange.right;
            newPmaSize = std::max(size, lastRebalancingTaskEnd);
            increaseSwapPmaSize(newPmaSize);
        }

        //std::cout << "Number of blocks to rebalance/rewrite: " << numberOfBlocksToRebalance << std::endl;
        if constexpr (SplitRebalancingTasks) {
            //B-5(a). Start each thread with the respective number of blocks to rewrite, splitting rebalancing tasks appropriately.
            const SizeType rewritingBlocksPerThread = numberOfBlocksToRebalance / numberOfThreads + 1;
            //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) firstprivate(rewritingBlocksPerThread, numberOfBlocksToRebalance)
            {
                const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
                const IndexType blockStart = static_cast<IndexType>(thread * rewritingBlocksPerThread);
                const IndexType blockEnd = std::min(numberOfBlocksToRebalance, static_cast<IndexType>((thread + 1) * rewritingBlocksPerThread));
                tasks[thread].rebalancingBlockRange = Interval(blockStart, blockEnd);
                if (tasks[thread].rebalancingBlockRange.length() > 0) {
                    const IndexType taskStart = findRebalancingTask(blockStart);
                    const IndexType taskEnd = findRebalancingTask(blockEnd) + 1;
                    tasks[thread].rebalancingTaskRange = Interval(taskStart, taskEnd);
                    //std::cout << "Thread " << thread <<  " rebalances blocks " << tasks[thread].rebalancingBlockRange << ", tasks " << tasks[thread].rebalancingTaskRange << std::endl;
                    //First compute all the block ranges
                    tasks[thread].firstRebalancingTask = getPartialRebalancingRange(taskStart, tasks[thread].rebalancingBlockRange);
                    if (tasks[thread].rebalancingTaskRange.length() > 1) tasks[thread].lastRebalancingTask = getPartialRebalancingRange(taskEnd - 1, tasks[thread].rebalancingBlockRange);
                }
            }
            if constexpr (Profile) profiler.endPhase(Helpers::Profiler::Phase::REBALANCING_DISTRIBUTION);

            if constexpr (Profile) profiler.startPhase(Helpers::Profiler::Phase::REBALANCING_EXECUTION);
            //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) firstprivate(rewritingBlocksPerThread, numberOfBlocksToRebalance)
            {
                const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
                //std::cout << "Executing rebalances for thread " << thread << std::endl;
                if (tasks[thread].rebalancingBlockRange.length() > 0) {
                    //After all threads are done, start the tasks
                    startPartialRebalancingTask(tasks[thread].rebalancingTaskRange.left, tasks[thread].firstRebalancingTask);
                    for (IndexType task = tasks[thread].rebalancingTaskRange.left + 1; task < tasks[thread].rebalancingTaskRange.right - 1; ++task) {
                        startFullRebalancingTask(task);
                    }
                    if (tasks[thread].rebalancingTaskRange.length() > 1) startPartialRebalancingTask(tasks[thread].rebalancingTaskRange.right - 1, tasks[thread].lastRebalancingTask);
                    //AssertMsg(numberOfBlocksRebalanced == tasks[thread].rebalancingBlockRange.length(), "Not written enough blocks!");
                }
            }
        } else {
            if constexpr (Profile) profiler.endPhase(Helpers::Profiler::Phase::REBALANCING_DISTRIBUTION);

            if constexpr (Profile) profiler.startPhase(Helpers::Profiler::Phase::REBALANCING_EXECUTION);
            //B-5(b). Start the rebalancing tasks, each task is entirely executed by one thread.
            const SizeType rebalancingTasksPerThread = numberOfRebalancingTasks / numberOfThreads + 1;
            //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) shared(std::cerr) firstprivate(numberOfRebalancingTasks, rebalancingTasksPerThread)
            {
                const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
                const IndexType firstRebalancingTask = static_cast<SizeType>(thread * rebalancingTasksPerThread);
                const IndexType lastRebalancingTask = std::min(numberOfRebalancingTasks, static_cast<SizeType>((thread + 1) * rebalancingTasksPerThread));
                for (IndexType rebalancingTask = firstRebalancingTask; rebalancingTask < lastRebalancingTask; ++rebalancingTask) {
                    startRebalancingTask(rebalancingTasks[rebalancingTask]);
                    rebalancingManager.clearRebalancedFlag(rebalancingTasks[rebalancingTask].rootNode);
                }
            }
        }
        AssertMsg(bufferReads == bufferWrites, "All buffer elements read");

        //B-6. If needed, reallocate the rebalancing manager.
        rebalancingManager.resize(newPmaSize);

        //B-7(a). If needed, clear the buffer pointers.
        if constexpr (SplitRebalancingTasks) {
            //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) firstprivate(numberOfBlocksToRebalance)
            {
                const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
                if (tasks[thread].rebalancingBlockRange.length() > 0) {
                    //Clear the buffer block pointers
                    startPartialClearTask(tasks[thread].firstRebalancingTask);
                    for (IndexType task = tasks[thread].rebalancingTaskRange.left + 1; task < tasks[thread].rebalancingTaskRange.right - 1; ++task) {
                        startFullClearTask(task);
                    }
                    if (tasks[thread].rebalancingTaskRange.length() > 1) startPartialClearTask(tasks[thread].lastRebalancingTask);
                }
            }
        } //else: nothing to do.

        //B-8. If needed, reallocate a larger array for the PMA.
        resizeMainPma(newPmaSize);
        AssertMsg(size == newPmaSize, "Resize failed");

#if !defined NDEBUG && EXTENDED_TESTS
        for (IndexType i = 0; i < capacity; ++i) {
            AssertMsg(bufferPointers[i].left == 0 && bufferPointers[i].right == 0, "Buffer pointers not cleared.");
        }
        for (IndexType i = 0; i < bufferCapacity; ++i) {
            AssertMsg(bufferArray[i].block == nullptr, "Buffer block not cleared.");
        }
#endif

        if constexpr (SplitRebalancingTasks) {
            //B-9(a). Start the rewriting tasks, splitting tasks for work balance.
            const SizeType rewritingBlocksPerThread = numberOfBlocksToRebalance / numberOfThreads + 1;
            //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) firstprivate(rewritingBlocksPerThread, numberOfBlocksToRebalance, numberOfRebalancingTasks)
            {
                const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
                const IndexType blockStart = static_cast<IndexType>(thread * rewritingBlocksPerThread);
                const IndexType blockEnd = std::min(numberOfBlocksToRebalance, static_cast<IndexType>((thread + 1) * rewritingBlocksPerThread));
                if (blockEnd > blockStart) {
                    const Interval blockRange(blockStart, blockEnd);
                    const IndexType taskStart = findRebalancingTask(blockStart);
                    const IndexType taskEnd = findRebalancingTask(blockEnd) + 1;
                    startPartialRewritingTask(taskStart, blockRange);
                    for (IndexType task = taskStart + 1; task < taskEnd - 1; ++task) {
                        startFullRewritingTask(task);
                    }
                    if (taskEnd - taskStart > 1) startPartialRewritingTask(taskEnd - 1, blockRange);
                }
            }
        } else {
            //B-9(b). Start the rewriting tasks, each task is entirely executed by one thread.
            const SizeType rebalancingTasksPerThread = numberOfRebalancingTasks / numberOfThreads + 1;
            //for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread)
#pragma omp parallel num_threads(numberOfThreads) default(none) firstprivate(numberOfRebalancingTasks, rebalancingTasksPerThread, newPmaSize)
            {
                const UnsignedIndexType thread = static_cast<UnsignedIndexType>(omp_get_thread_num());
                const IndexType firstRebalancingTask = std::min(numberOfRebalancingTasks, static_cast<SizeType>(thread * rebalancingTasksPerThread));
                const IndexType lastRebalancingTask = std::min(numberOfRebalancingTasks, static_cast<SizeType>((thread + 1) * rebalancingTasksPerThread));
                for (IndexType rebalancingTask = firstRebalancingTask; rebalancingTask < lastRebalancingTask; ++rebalancingTask) {
                    startRewritingTask(rebalancingTasks[rebalancingTask], newPmaSize);
                }
            }
        }

#if !defined NDEBUG && EXTENDED_TESTS
        for (IndexType i = 0; i < swapCapacity; ++i) {
            AssertMsg(swapPma[i].block == nullptr, "Swap pma block not cleared.");
        }
        for (IndexType i = 0; i < bufferCapacity; ++i) {
            AssertMsg(bufferArray[i].block == nullptr, "Buffer block not cleared.");
        }
        for (IndexType i = 0; i < capacity; ++i) {
            AssertMsg(bufferPointers[i].left == 0 && bufferPointers[i].right == 0, "Buffer pointers not cleared.");
        }
#endif

        //B-10. Clear the rebalancing information.
        rebalancingTasks.clear();
        prefixNumberOfRebalancingBlocks.clear();

        if constexpr (Profile) profiler.endPhase(Helpers::Profiler::Phase::REBALANCING_EXECUTION);

#if !defined NDEBUG && EXTENDED_TESTS
        isValid();
#endif
        if constexpr (Profile) profiler.endOperation(Helpers::Profiler::Operation::INSERT);
        //std::cout << "end batch insert" << std::endl << std::endl << std::endl << std::endl << std::endl;
    }

    void removeBatch(Pointer batch, SizeType batchSize, UnsignedIndexType numThreads) {
        (void) batch;
        (void) batchSize;
        (void) numThreads;
    }

    bool contains(const KeyType e) {//Could be const without profiler
        IndexType blockIndex = findContainingBlock(e);
        bool result = blockIndex >= 0 && !pma[blockIndex].isEmpty() && pma[blockIndex].contains(e);
        return result;
    }

    /**
     * Returns an iterator to the entry with the given key, iff it exists.
     * Otherwise, returns end().
     */
    iterator find(const KeyType e) {//Could be const without profiler
        IndexType blockIndex = findContainingBlock(e);
        if (blockIndex < 0 || pma[blockIndex].isEmpty()) {
            return end();
        }
        const BlockIndexType indexInBlock = pma[blockIndex].findIndex(e);
        if (indexInBlock >= blockSize(blockIndex) || KeyOf::keyOfValue(pma[blockIndex].get(indexInBlock)) != e) {
            return end();
        }
        return iterator(this, blockIndex, indexInBlock);
    }

    /**
     * Returns an iterator to the first entry whose key does not satisfy
     * comp(KeyOf::keyOfValue(entry), e).
     */
    iterator lowerBound(const KeyType e) {//Could be const without profiler
        IndexType blockIndex = findBlockIndex(e);
        if (blockIndex >= size) {
            //The key is larger than all elements in the data structure.
            return end();
        }
        const BlockIndexType indexInBlock = pma[blockIndex].findIndex(e);
        if (indexInBlock >= blockSize(blockIndex)) {
            //The element is larger than all elements in the block, return iterator to the start of the next block.
            blockIndex = findNext(blockIndex, size);
            if (blockIndex >= size) {
                //There is no further block.
                return end();
            }
            AssertMsg(!empty(blockIndex), "Returning an invalid iterator.");
            return iterator(this, blockIndex, 0);
        }
        return iterator(this, blockIndex, indexInBlock);
    }

    template<typename EVALUATOR>
    void rangeQuery(KeyType left, KeyType right, EVALUATOR f) {//Could be const without profiler
        iterator current = lowerBound(left);

        while (current != end() && comp(KeyOf::keyOfValue(*current), right)) {
            AssertMsg((comp(left, KeyOf::keyOfValue(*current)) || left == KeyOf::keyOfValue(*current)) && comp(KeyOf::keyOfValue(*current), right), "Range query considers an invalid value");
            f(*current);
            ++current;
        }
    }

private:
    /**
     * Inserts the range batchRange of the given batch,
     * which corresponds to pmaInputRange in the PMA,
     * writing new blocks into the range bufferRange of the buffer array.
     *
     * Returns the number of blocks that was written to the buffer array.
     */
    SizeType insertBatchRange(Pointer batch, const Interval &batchRange, const Interval &pmaInputRange, const Interval &bufferRange) {
        IndexType batchIndex = batchRange.left;
        IndexType blockIndex = pmaInputRange.left;//Start the insertion at the beginning of the input range.
        AssertMsg(pmaInputRange.length() == 0 || !empty(blockIndex), "Input batch starts on an empty block");
        IndexType nextBlockIndex;
        IndexType outputBlockNumber = 0;//Keep track of which block we are in to calculate the write indices.

        //Special case for the initial insertion.
        if (pmaInputRange.length() == 0) {
            outputBlockNumber = insertIntoEmptyRange(batch, batchRange, batchIndex, pmaInputRange, bufferRange, outputBlockNumber);
            return outputBlockNumber;
        }
        while (batchIndex < batchRange.right) {
            //Use exponential search to find the block that the next batch element goes into
            blockIndex = exponentialSearch(KeyOf::keyOfValue(batch[batchIndex]), blockIndex, pmaInputRange.right);
            nextBlockIndex = findNext(blockIndex, pmaInputRange.right);
            AssertMsg(blockIndex == linearSearch(KeyOf::keyOfValue(batch[batchIndex]), blockIndex, pmaInputRange.right), "Wrong exponential search results");
            outputBlockNumber = insertElementsIntoBlock(blockIndex, nextBlockIndex, pmaInputRange, bufferRange, batch, batchIndex, batchRange, outputBlockNumber);
        }
        AssertMsg(batchIndex == batchRange.right, "Entire batch was inserted.");
        return outputBlockNumber;
    }

    [[nodiscard]] IndexType inverseMerge(const Pointer &batch, const Interval &batchRange, const Interval &mergeRange, const Interval &pmaInputRange, const Interval& bufferRange) {
        AssertMsg(std::max(batchRange.length(), mergeRange.length()) >= MinBlockSize, "Not enough elements for inverse merge task");
        //std::cout << "Inverse merge task" << std::endl;
        IndexType outputBlockNumber = 0;
        PmaEntryType insertionBlock = PmaEntryType();
        const IndexType mergeBlock = pmaInputRange.left;
        insertionBlock.allocateBlock(allocator);
        IndexType batchIndex = batchRange.left;
        BlockIndexType readIndex = static_cast<BlockIndexType>(mergeRange.left);
        KeyType lastWrittenKey = NULL_KEY;
        while (batchIndex < batchRange.right || static_cast<IndexType>(readIndex) < mergeRange.right) {
            if (insertionBlock.size == BlockSize) {
                //Block is full, write it to output and get a new one
                PmaEntryType newBlock{};
                const SizeType remainingBatchElements = batchRange.right - batchIndex;
                const SizeType remainingBlockElements = mergeRange.right - static_cast<IndexType>(readIndex);
                //Number of elements that will certainly be inserted, even if all others are duplicates
                const SizeType minRemainingElements = std::max(remainingBatchElements, remainingBlockElements);
                if (minRemainingElements > 0 && minRemainingElements < MinBlockSize) {
                    //The remaining elements would not be sufficient to fill the next block, split the last one
                    newBlock = insertionBlock.split(allocator);
                } else {
                    newBlock.allocateBlock(allocator);
                }
                writeToBufferArray(insertionBlock, bufferRange, outputBlockNumber, mergeBlock);
                ++outputBlockNumber;
                insertionBlock = newBlock;
            }
            if (batchIndex < batchRange.right && static_cast<IndexType>(readIndex) < mergeRange.right) {
                //Insert the smaller of the two possible elements into the block. Prefer the element from mergeBlock on equality.
                if (comp(KeyOf::keyOfValue(pma[mergeBlock].get(readIndex)), KeyOf::keyOfValue(batch[batchIndex]))) {
                    //Insert from the mergeBlock
                    insertionBlock.insertWithIndex(pma[mergeBlock].get(readIndex), insertionBlock.size);
                    lastWrittenKey = KeyOf::keyOfValue(pma[mergeBlock].get(readIndex));
                    ++readIndex;
                    while (batchIndex < batchRange.right && KeyOf::keyOfValue(batch[batchIndex]) == lastWrittenKey) {
                        ++batchIndex;//Skip duplicates at the start of the batch range.
                    }
                } else if (KeyOf::keyOfValue(pma[mergeBlock].get(readIndex)) == KeyOf::keyOfValue(batch[batchIndex])) {
                    //Insert from the merge block, ignore the current batch element (duplicate key)
                    insertionBlock.insertWithIndex(pma[mergeBlock].get(readIndex), insertionBlock.size);
                    lastWrittenKey = KeyOf::keyOfValue(pma[mergeBlock].get(readIndex));
                    ++readIndex;
                    ++batchIndex;
                    while (batchIndex < batchRange.right && KeyOf::keyOfValue(batch[batchIndex]) == lastWrittenKey) {
                        ++batchIndex;//Skip duplicates at the start of the batch range.
                    }
                } else {
                    //Insert from the batch
                    if (KeyOf::keyOfValue(batch[batchIndex]) != lastWrittenKey) {
                        insertionBlock.insertWithIndex(batch[batchIndex], insertionBlock.size);
                        lastWrittenKey = KeyOf::keyOfValue(batch[batchIndex]);
                    }
                    ++batchIndex;
                }
            } else if (batchIndex < batchRange.right && static_cast<IndexType>(readIndex) >= mergeRange.right) {
                //Merge as many elements from the remaining part of the batch as possible
                IndexType numberOfInsertions = std::min(static_cast<IndexType>(BlockSize - insertionBlock.size), batchRange.right - batchIndex);
                insertionBlock.merge(batch + batchIndex, static_cast<BlockSizeType>(numberOfInsertions));
                batchIndex += numberOfInsertions;
            } else if (batchIndex >= batchRange.right && static_cast<IndexType>(readIndex) < mergeRange.right) {
                //Merge the remaining part of the block
                BlockSizeType numberOfInsertions = std::min(static_cast<BlockSizeType>(BlockSize - insertionBlock.size), static_cast<BlockSizeType>(mergeRange.right - readIndex));
                insertionBlock.merge(pma[mergeBlock].block->begin() + readIndex, numberOfInsertions);
                readIndex += numberOfInsertions;
            } else {
                AssertMsg(false, "Should not happen!");
            }
            AssertMsg(batchIndex < batchRange.right || static_cast<IndexType>(readIndex) < mergeRange.right || insertionBlock.size >= MinBlockSize || insertionBlock.size == 0, "Invalid block size.");
        }
        if (insertionBlock.size > 0) {
            writeToBufferArray(insertionBlock, bufferRange, outputBlockNumber, mergeBlock);
            ++outputBlockNumber;
        } else {
            insertionBlock.deleteBlock(allocator);
        }
        AssertMsg(batchIndex == batchRange.right, "Entire batch was inserted.");
        AssertMsg(readIndex == mergeRange.right, "Entire merge block section was inserted.");
        return outputBlockNumber;
    }

    /**
     * Returns the number of the next block to be written to the pma.
     */
    [[nodiscard]] IndexType insertElementsIntoBlock(IndexType blockIndex, IndexType nextBlockIndex, const Interval &pmaInputRange, const Interval &bufferRange, Pointer batch, IndexType &batchIndex, Interval batchRange, IndexType outputBlockNumber) {
        const IndexType lastBlockElement = findLastBatchElementForBlock(batch, batchIndex, batchRange, nextBlockIndex, pmaInputRange);
        const SizeType elementsInBlock = lastBlockElement - batchIndex;
        //Check if these elements fit into the block
        AssertMsg(elementsInBlock > 0, "Inserting 0 elements into a block");
        if (blockSize(blockIndex) + elementsInBlock <= BlockSize) {
            //There is enough space in the block. Merge the elements into the block directly. Do not write any blocks to the buffer.
            insertIntoBlockWithoutSplit(blockIndex, batch, batchIndex, lastBlockElement);
        } else {
            outputBlockNumber = insertIntoBlockWithSplit(blockIndex, batch, batchIndex, lastBlockElement, bufferRange, outputBlockNumber);
        }
        return outputBlockNumber;
    }

    void insertIntoBlockWithoutSplit(IndexType blockIndex, Pointer batch, IndexType &batchIndex, IndexType lastBlockElement) {
        //const BlockSizeType oldSize = blockSize(blockIndex);
        const BlockSizeType numberOfInsertions = static_cast<BlockSizeType>(lastBlockElement - batchIndex);
        pma[blockIndex].merge(batch + batchIndex, numberOfInsertions);
        batchIndex = lastBlockElement;
    }

    /**
     * Returns the number of the next block to be written to the pma.
     */
    [[nodiscard]] IndexType insertIntoBlockWithSplit(const IndexType blockIndex, const Pointer batch, IndexType &batchIndex, const IndexType lastBlockElement, const Interval &bufferRange, IndexType outputBlockNumber) {
        //The elements that need to go into this block do not fit here.
        //Merge until the block is full, then split the upper half of the  elements of the block into a new block
        //and continue merging there. Repeat until all elements from the batch that go into the block are merged.
        PmaEntryType insertionBlock = pma[blockIndex];
        pma[blockIndex] = PmaEntryType();
        //Not necessary to mark update of the hierarchy here: Will happen when blocks are written.
        AssertMsg(insertionBlock.size == 0 || insertionBlock.head == KeyOf::keyOfValue(insertionBlock.get(0)), "Wrong head");
        BlockIndexType lastInsertionIndex = 0;
        PmaEntryType rightHalf = PmaEntryType();
        //Initialize an empty range in the buffer array.
        bufferPointers[blockIndex].left = getDenseWriteIndex(bufferRange.left, outputBlockNumber);
        bufferPointers[blockIndex].right = bufferPointers[blockIndex].left;
        while (batchIndex < lastBlockElement) {
            AssertMsg(insertionBlock.size == 0 || insertionBlock.head == KeyOf::keyOfValue(insertionBlock.get(0)), "Wrong head");
            if (!rightHalf.isEmpty() && (comp(rightHalf.head, KeyOf::keyOfValue(batch[batchIndex])) || KeyOf::keyOfValue(batch[batchIndex]) == rightHalf.head)) {
                //The next batch element goes to the rightHalf.
                //Write the current insertion block and switch the roles of the two blocks.
                writeToBufferArray(insertionBlock, bufferRange, outputBlockNumber, blockIndex);
                ++outputBlockNumber;
                insertionBlock = rightHalf;
                rightHalf = PmaEntryType();
                lastInsertionIndex = 0;
            }
            if (insertionBlock.size < BlockSize) {
                std::tie(std::ignore, lastInsertionIndex) = insertionBlock.insertWithIndex(batch[batchIndex], lastInsertionIndex);
                AssertMsg(insertionBlock.size == 0 || insertionBlock.head == KeyOf::keyOfValue(insertionBlock.get(0)), "Wrong head");
                ++batchIndex;
            } else {
                //Not enough space left, split the block
                PmaEntryType newBlock = insertionBlock.split(allocator);
                if (!rightHalf.isEmpty()) {
                    //There was already a block split off. We know that the next insertion can no longer go to insertion block, so switch to newBlock as insertionBlock
                    writeToBufferArray(insertionBlock, bufferRange, outputBlockNumber, blockIndex);
                    ++outputBlockNumber;
                    insertionBlock = newBlock;
                    lastInsertionIndex = 0;
                } else {
                    //Keep newBlock as rightHalf.
                    rightHalf = newBlock;
                }
            }
            AssertMsg(pma[blockIndex].size == 0 || pma[blockIndex].head == KeyOf::keyOfValue(pma[blockIndex].get(0)), "Wrong head");
            AssertMsg(insertionBlock.size == 0 || insertionBlock.head == KeyOf::keyOfValue(insertionBlock.get(0)), "Wrong head");
        }
        //Write insertion block
        writeToBufferArray(insertionBlock, bufferRange, outputBlockNumber, blockIndex);
        ++outputBlockNumber;
        if (!rightHalf.isEmpty()) {
            //Write remaining right half to the output, too.
            writeToBufferArray(rightHalf, bufferRange, outputBlockNumber, blockIndex);
            ++outputBlockNumber;
        }
        //Keep the end of the range of the new blocks in the buffer array
        bufferPointers[blockIndex].right = getDenseWriteIndex(bufferRange.left, outputBlockNumber - 1) + 1;
        AssertMsg(bufferPointers[blockIndex].left >= 0 && bufferPointers[blockIndex].right >= bufferPointers[blockIndex].left, "Invalid buffer range");
        return outputBlockNumber;
    }

    /**
     * Returns the number of the next block to be written to the pma.
     */
    [[nodiscard]] IndexType insertIntoEmptyRange(Pointer batch, const Interval &batchRange, IndexType &batchIndex, const Interval &inputRange, const Interval &bufferRange, IndexType outputBlockNumber) {
        //std::cout << "Insert into empty range." << std::endl;
        //The pma range was empty, we need to create all new blocks
        PmaEntryType insertionBlock = PmaEntryType();
        insertionBlock.allocateBlock(allocator);
        while (batchIndex < batchRange.right) {
            if (insertionBlock.size == BlockSize) {
                //Block is full, write it to output and get a new one
                PmaEntryType newBlock{};
                SizeType remainingElements = batchRange.right - batchIndex;
                if (remainingElements > 0 && remainingElements < MinBlockSize) {
                    //The remaining elements would not be sufficient to fill the next block, split the last one
                    newBlock = insertionBlock.split(allocator);
                } else {
                    newBlock.allocateBlock(allocator);
                }
                writeToBufferArray(insertionBlock, bufferRange, outputBlockNumber, inputRange.left);
                ++outputBlockNumber;
                insertionBlock = newBlock;
                if (batchIndex >= batchRange.right) {
                    batchIndex = batchRange.right;
                    break;
                }
            }
            //Insert as many elements into the block as possible
            IndexType numberOfInsertions = std::min(static_cast<IndexType>(BlockSize), batchRange.right - batchIndex);
            insertionBlock.merge(batch + batchIndex, static_cast<BlockSizeType>(numberOfInsertions));
            batchIndex += numberOfInsertions;
        }
        if (insertionBlock.size > 0) {
            writeToBufferArray(insertionBlock, bufferRange, outputBlockNumber, inputRange.left);
            ++outputBlockNumber;
        } else {
            insertionBlock.deleteBlock(allocator);
        }
        AssertMsg(batchIndex == batchRange.right, "Entire batch was inserted.");
        bufferPointers[inputRange.left].left = getDenseWriteIndex(bufferRange.left, 0);
        bufferPointers[inputRange.left].right = getDenseWriteIndex(bufferRange.left, outputBlockNumber - 1) + 1;
        return outputBlockNumber;
    }

    [[nodiscard]] IndexType findLastBatchElementForBlock(Pointer batch, IndexType batchIndex, const Interval &batchRange, IndexType nextBlockIndex, const Interval &pmaRange) const {
        //Find the last element from the batch that goes into the current block
        if (nextBlockIndex == pmaRange.right) {
            return batchRange.right;
        }
        IndexType lastBlockElement = batchIndex;
        while (lastBlockElement < batchRange.right && comp(KeyOf::keyOfValue(batch[lastBlockElement]), head(nextBlockIndex))) {
            ++lastBlockElement;
        }
        return lastBlockElement;
    }

    /**
     * Uses exponentialSearch search to find the last block in the range [lower, upper)
     * that has head <= e.
     * Returns the index of the desired block
     */
    IndexType exponentialSearch(KeyType e, IndexType lower, IndexType upper) {
        IndexType start = lower;
        IndexType end = lower;
        while(end < upper && empty(end)) ++end;
        SizeType delta = 1;
        //Use exponential search to find first entry that has head > e...
        while (end < upper && (comp(head(end), e) || head(end) == e)) {
            start = end;
            end = findNext(end + delta, upper);
            delta *= 2;
        }
        return binarySearch(e, start, std::min(upper, end));//Then use binary search in the last interval
    }

    /**
     * Uses binary search to find the last block in the range [lower, upper)
     * that has head <= e.
     * Returns the index of the desired block
     */
    IndexType binarySearch(KeyType e, IndexType lower, IndexType upper) {
        IndexType left = lower, right = upper - 1;//inclusive boundaries
        IndexType candidate = -1;
        while (right - left > BinarySearchCutoff) {
            const IndexType mid = (left + right) / 2;
            IndexType prev = mid;
            while (prev >= left && empty(prev)) --prev;
            if (prev < left) {
                left = mid + 1;//No non-empty block in the left half
            } else {
                if (head(prev) == e) {
                    AssertMsg(prev == linearSearch(e, lower, upper), "Binary search was wrong");
                    return prev;//Found the block
                } else if (comp(e, head(prev))) {
                    //The current head is larger than the element, go left
                    right = prev - 1;
                } else {
                    //Go right
                    left = mid + 1;
                    //If it turns out that there  is no more element to the right, take prev as it is the first larger one.
                    candidate = prev;
                }
            }
        }
        auto result = linearSearch(e, left, upper);
        if (empty(result) || (comp(e, head(result)) && result != lower)) {
            //The result we got was wrong, there was no more element to the left, use candidate
            AssertMsg(candidate != -1 && !empty(candidate), "Candidate must be set");
            AssertMsg(candidate == linearSearch(e, lower, upper), "Binary search was wrong");
            return candidate;
        }
        AssertMsg(result == linearSearch(e, lower, upper), "Binary search was wrong");
        return result;
    }

    /**
     * Uses linear search to find the last block in the range [lower, upper)
     * that has head <= e.
     * Returns the index of the desired block.
     */
    IndexType linearSearch(KeyType e, IndexType lower, IndexType upper) {
        IndexType firstLarger = findNext(lower, upper);
        while (firstLarger < upper && !empty(firstLarger) && (comp(head(firstLarger), e) || e == head(firstLarger))) {
            //Go to next block.
            firstLarger = findNext(firstLarger, upper);
        }
        return findPrevious(firstLarger, lower);
    }

    /**
     * Finds the first index <= startIndex such that the batch element at that index is >= blockHead, if it exists.
     * Otherwise, returns startIndex + 1.
     */
    IndexType findFirstBatchElement(Pointer batch, IndexType startIndex, KeyType blockHead) {
        //Use exponential search to find some batch element that is < blockHead
        SizeType delta = 1;
        IndexType current = startIndex - delta;
        while (current > 0 && (comp(blockHead, KeyOf::keyOfValue(batch[current])) || blockHead == KeyOf::keyOfValue(batch[current]))) {
            delta *= 2;
            current -= delta;
        }
        if (current < 0) current = 0;

        //Use binary search in the last interval to find the first batch element that is >= blockHead, if it exists.
        IndexType result = std::lower_bound(batch + current, batch + current + delta + 1, blockHead, [this](const ValueType &lhs, const KeyType &rhs) {
            return comp(KeyOf::keyOfValue(lhs), rhs);
        }) - batch;

        return result;
    }

    /**
     * Finds the next larger entry that has a non-empty entry and is < upperLimit.
     */
    IndexType findNext(IndexType start, IndexType upperLimit) const {
        IndexType current = start + 1;
        while(current < upperLimit && empty(current)) ++current;
        return current;
    }

    /**
     * Finds the next smaller entry that has a non-empty entry and is >= lowerLimit.
     */
    IndexType findPrevious(IndexType start, IndexType lowerLimit) const {
        IndexType current = start - 1;
        while(current > lowerLimit && empty(current)) --current;
        return current;
    }

    /**
     * Finds the index of the block that the given element should be contained in.
     * Always returns an index of a non-empty position.
     */
    IndexType findBlockIndex(KeyType e) {
        const IndexType containingBlock = findContainingBlock(e);
        if (containingBlock == -1) {
            //Element is smaller than all others, find the first non-empty block for it
            IndexType firstBlock = 0;
            while (firstBlock < size && empty(firstBlock)) ++firstBlock;
            return firstBlock;
        }
        return containingBlock;
    }

    /**
     * Finds the index of the block that has to contain the element, if it exists.
     */
    IndexType findContainingBlock(KeyType e) {
        IndexType left = 0, right = size - 1;//inclusive boundaries
        IndexType candidate = -1;
        while (right - left > BinarySearchCutoff) {
            const IndexType mid = (left + right) / 2;
            IndexType prev = mid;
            while (prev >= left && empty(prev)) --prev;
            if (prev < left) {
                left = mid + 1;
            } else {
                if (comp(e, head(prev))) {
                    right = prev - 1;
                } else if (comp(head(prev), e)) {
                    candidate = prev;//Prev is still possible. If we do not find anything else, take it.
                    left = prev + 1;
                } else {//head(prev) == e
                    return prev;//Found the block
                }
            }
        }
        //Linear search the result:
        IndexType result = linearSearch(e, left, right + 1);
        if (empty(result) || comp(e, head(result))) {
            if (candidate != -1) {
                //The result we got was wrong, there was no more element to the left, use candidate
                AssertMsg(candidate != -1 && !empty(candidate), "Candidate must be set");
                return candidate;
            } else {
                //Element is smaller than all others.
                return -1;
            }
        }
        return result;
    }

    void distributeWorkInMergedTasks(Pointer batch, SizeType batchSize) {
        tasks[0].mergedTasks = 0;
        for (UnsignedIndexType thread = 1; thread < numberOfThreads; ++thread) {
            //std::cout << "Distributing for thread " << thread << std::endl;
            if (tasks[thread].type == TaskType::INSERTION && tasks[thread].mergedTasks > 0) {
                //std::cout << "   Is last task of inverse merge group." << std::endl;
                //The current task is the last task of a row of merged tasks.
                //Make this task do the work of all the remaining blocks,
                //and all the previous tasks to the work of the one block for which they were merged.
                const IndexType mergeBlock = tasks[thread].blockForBatchSplitter;
                const Interval totalBatchRange(tasks[thread].firstBatchElement, tasks[thread + 1].firstBatchElement);
                const Interval pmaInputRange(tasks[thread].blockForBatchSplitter, tasks[thread + 1].blockForBatchSplitter);
                const IndexType firstNormalBlock = findNext(mergeBlock, pmaInputRange.right);
                /*
                std::cout << "Last task " << thread << " has " << tasks[thread].mergedTasks << " merged tasks." << std::endl;
                std::cout << "Last task " << thread << " has batch range " << totalBatchRange << std::endl;
                std::cout << "Last task " << thread << " has pma Input range " << pmaInputRange << std::endl;
                std::cout << "mergeBlock: " << mergeBlock << " with head" << head(mergeBlock) << std::endl;
                std::cout << "firstNormalBlock: " << firstNormalBlock << " with head" << head(firstNormalBlock) << std::endl;
                */
                IndexType lastTaskBatchStart;
                if (empty(firstNormalBlock)) {
                    //The mergeBlock is the last non-empty block overall, the last task does not get any work
                    lastTaskBatchStart = totalBatchRange.right;
                } else {
                    //1. Let this task only do batch elements that do not concern the mergeBlock
                    //the lastTaskBatchStart should be the first batch element that is >= head(firstNormalBlock)
                    lastTaskBatchStart = std::lower_bound(batch, batch + batchSize, head(firstNormalBlock), [](const ValueType &val, const KeyType &nextHead) -> bool {
                        return KeyOf::keyOfValue(val) < nextHead;
                    }) - batch;
                    lastTaskBatchStart = std::min(lastTaskBatchStart, totalBatchRange.right);
                }
                AssertMsg(lastTaskBatchStart >= totalBatchRange.left && lastTaskBatchStart <= totalBatchRange.right, "Illegal new batch start for last task");
                const Interval mergeBatchRange(totalBatchRange.left, lastTaskBatchStart);
                if (mergeBatchRange.length() >= MinBlockSize) {
                    //There is enough work for the merged tasks, adjust the last task appropriately
                    tasks[thread].firstBatchElement = lastTaskBatchStart;
                    tasks[thread].blockForBatchSplitter = firstNormalBlock;
                } else {
                    //There is not enough work for even only one merge task, just do everything in the last task.
                    for (UnsignedIndexType mergeThread = tasks[thread].mergedTasks; mergeThread > 0; --mergeThread) {
                        //Make thread nr. thread - mergeThread do nothing
                        //std::cout << "   Task " << (thread - mergeThread) << " does nothing" << std::endl;
                        tasks[thread - mergeThread].type = TaskType::DO_NOTHING;
                        tasks[thread - mergeThread].firstBatchElement = mergeBatchRange.left;
                        tasks[thread - mergeThread].blockForBatchSplitter = mergeBlock;
                        tasks[thread - mergeThread].mergedTasks = 0;
                        tasks[thread - mergeThread].mergedTaskIndex = 0;
                        tasks[thread - mergeThread].requiredWidth = 0;
                    }

                    tasks[thread].mergedTasks = 0;
                    //std::cout << "      Not enough work!" << std::endl;
                    continue;
                }
                //AssertMsg(numberOfElements() == 0 || !empty(mergeBlock), "Merge block is empty");

                //2. Distribute the remaining part of the batch to the mergedTasks many tasks in front of this one, give them all the merged block
                //Use as many tasks as possible, while giving each task enough elements for at least one block
                const UnsignedSizeType numberOfTasksWithWork = std::min(tasks[thread].mergedTasks, static_cast<UnsignedSizeType>(mergeBatchRange.length() / MinBlockSize));
                AssertMsg(numberOfTasksWithWork >= 1, "There is at least one task that does work.");
                const IndexType elementsPerThread = mergeBatchRange.length() / numberOfTasksWithWork;
                //std::cout << "Number of tasks with work: " << numberOfTasksWithWork << std::endl;
                //std::cout << "Elements per thread: " << elementsPerThread << std::endl;
                for (UnsignedIndexType mergeThread = tasks[thread].mergedTasks; mergeThread > numberOfTasksWithWork; --mergeThread) {
                    //Make thread nr. thread - mergeThread do nothing
                    //std::cout << "   Task " << (thread - mergeThread) << " does nothing" << std::endl;
                    tasks[thread - mergeThread].type = TaskType::DO_NOTHING;
                    tasks[thread - mergeThread].firstBatchElement = mergeBatchRange.left;
                    tasks[thread - mergeThread].blockForBatchSplitter = mergeBlock;
                    tasks[thread - mergeThread].mergedTasks = numberOfTasksWithWork;
                    tasks[thread - mergeThread].mergedTaskIndex = numberOfTasksWithWork - mergeThread;
                    tasks[thread - mergeThread].requiredWidth = 0;
                }
                for (UnsignedIndexType mergeThread = numberOfTasksWithWork; mergeThread > 0; --mergeThread) {
                    //Make thread nr. thread - mergeThread do a part of the work
                    //std::cout << "   Task " << (thread - mergeThread) << " is inverse merge task with index " << (numberOfTasksWithWork - mergeThread) << std::endl;
                    const IndexType mergeTaskBatchStart = std::min(mergeBatchRange.left + (numberOfTasksWithWork - mergeThread) * elementsPerThread, static_cast<UnsignedIndexType>(mergeBatchRange.right));
                    AssertMsg(mergeTaskBatchStart == 0 || mergeBatchRange.right - mergeTaskBatchStart >= MinBlockSize, "Not enough elements left for last task");
                    tasks[thread - mergeThread].firstBatchElement = mergeTaskBatchStart;
                    tasks[thread - mergeThread].blockForBatchSplitter = mergeBlock;
                    tasks[thread - mergeThread].mergedTasks = numberOfTasksWithWork;
                    tasks[thread - mergeThread].mergedTaskIndex = numberOfTasksWithWork - mergeThread;
                    tasks[thread - mergeThread].requiredWidth = getRequiredWidth(elementsPerThread);
                }
                for (UnsignedIndexType mergeThread = numberOfTasksWithWork; mergeThread > 0; --mergeThread) {
                    const SizeType elementsInThread = tasks[thread - mergeThread + 1].firstBatchElement - tasks[thread - mergeThread].firstBatchElement;
                    tasks[thread - mergeThread].requiredWidth = getRequiredWidth(elementsInThread);
                }
            }
        }
    }

    SizeType getRequiredWidth(SizeType numberOfNewElements) {
        return 2 * numberOfNewElements;
    }

    void startInsertionTask(UnsignedIndexType taskIndex, Pointer batch, SizeType batchSize) {
        Interval batchRange(tasks[taskIndex].firstBatchElement, tasks[taskIndex + 1].firstBatchElement);
        Interval pmaInputRange(tasks[taskIndex].blockForBatchSplitter, tasks[taskIndex + 1].blockForBatchSplitter);
        Interval bufferRange(tasks[taskIndex].startIndex, tasks[taskIndex + 1].startIndex);
        if (tasks[taskIndex].type == TaskType::INSERTION) {
            if (batchRange.length() == 0) {
                tasks[taskIndex].numberOfBlocksWritten = 0;
                return;
            }
            if constexpr (Debug) std::cout << "Starting task " << taskIndex << ", length " << pmaInputRange.length() << ", --> " << bufferRange.length() << std::endl;
            tasks[taskIndex].numberOfBlocksWritten = insertBatchRange(batch, batchRange, pmaInputRange, bufferRange);
        } else if (tasks[taskIndex].type == TaskType::INVERSE_MERGE) {
            AssertMsg(UseInverseMerge, "illegal task type.");
            AssertMsg(tasks[taskIndex].type == TaskType::INVERSE_MERGE, "Wrong task type.");
            if (batchRange.length() == 0) {
                tasks[taskIndex].numberOfBlocksWritten = 0;
                return;//Nothing to do
            }
            const IndexType mergeBlock = pmaInputRange.left;
            Interval mergeRange(0, pma[mergeBlock].size);
            //Merge elements that are >= the batch start, of from the start of the block for the first subtask
            if (pma[mergeBlock].size != 0 && tasks[taskIndex].mergedTaskIndex != 0) {
                AssertMsg(batchRange.left != batchSize, "There is a valid batch element to be inserted.");
                mergeRange.left = pma[mergeBlock].findIndex(KeyOf::keyOfValue(batch[batchRange.left]));
            }
            //Merge all elements that are smaller than the next subtask's head, or to the end of the block for the last subtask
            if (pma[mergeBlock].size != 0 && tasks[taskIndex].mergedTaskIndex + 1 != tasks[taskIndex].mergedTasks) {
                if (batchRange.right != batchSize) {
                    mergeRange.right = pma[mergeBlock].findIndex(KeyOf::keyOfValue(batch[batchRange.right]));
                }
                //Otherwise: Keep mergeRange.right = merge block size; all the following tasks have batchRange.length() == 0,
                //So there is no work for them.
            }
            tasks[taskIndex].numberOfBlocksWritten = inverseMerge(batch, batchRange, mergeRange, pmaInputRange, bufferRange);
        } else if (tasks[taskIndex].type == TaskType::DO_NOTHING) {
            tasks[taskIndex].numberOfBlocksWritten = 0;
        }
    }

    void postprocessInverseMergeTasks() {
        //Postprocessing: Merge blocks that are too small and remove the blocks that were merged inversely.
        for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread) {
            if (thread > 0 && tasks[thread].type == TaskType::INSERTION && tasks[thread].mergedTasks > 0 && tasks[thread - 1].type == TaskType::INVERSE_MERGE) {
                //Task thread is the last task of a merge group, we can safely delete the mergeBlock here.
                const IndexType mergeBlock = tasks[thread - 1].blockForBatchSplitter;
                if (!empty(mergeBlock)) {//Merge block is only empty if the sds is empty.
                    //std::cout << "For thread " << thread << ", removing  inverse merge block " << pma[mergeBlock] << std::endl;
                    pma[mergeBlock].deleteBlock(allocator);
                    pma[mergeBlock] = PmaEntryType();//Null the entry
                    //Not necessary to mark mergeBlock node in the hierarchy: Blocks will be written there.
                }
            }
        }
    }

    void compactifyBufferBlocks() {
        IndexType bufferTargetIndex = -1;
        IndexType mergeBlock = -1;
        IndexType bufferPointerStart = -1;
        for (UnsignedIndexType thread = 0; thread < numberOfThreads; ++thread) {
            if (tasks[thread].type == TaskType::INVERSE_MERGE && tasks[thread].mergedTasks > 0) {
                const Interval bufferRange(tasks[thread].startIndex, tasks[thread + 1].startIndex);
                const SizeType numberOfBlocksWritten = tasks[thread].numberOfBlocksWritten;
                //std::cout << "Task " << thread << " has written " << tasks[thread].numberOfBlocksWritten << " blocks into buffer range " << bufferRange << std::endl;
                //std::cout << "Merge group size is " << tasks[thread].mergedTasks << std::endl;
                if (bufferTargetIndex == -1) {
                    //This is the first inverse merge thread of its group
                    bufferTargetIndex = bufferRange.left + numberOfBlocksWritten;
                    mergeBlock = tasks[thread].blockForBatchSplitter;
                    bufferPointerStart = bufferRange.left;
                    continue;
                }
                for (IndexType i = 0; i < numberOfBlocksWritten; ++i) {
                    //Write the next block to the front
                    AssertMsg(bufferArray[bufferRange.left + i].size > 0, "Buffer block is not there.");
                    AssertMsg(bufferArray[bufferTargetIndex].size == 0, "Buffer target is not empty.");
                    //std::cout << "Copying block from " << (bufferRange.left + i) << " to " << bufferTargetIndex << std::endl;
                    bufferArray[bufferTargetIndex] = bufferArray[bufferRange.left + i];
                    bufferArray[bufferRange.left + i] = PmaEntryType{};
                    ++bufferTargetIndex;
                }
            } else {
                if (mergeBlock != -1) {
                    if (tasks[thread].blockForBatchSplitter == mergeBlock) {
                        //This task is the last one of the merge group. Copy its blocks too.
                        const Interval bufferRange(tasks[thread].startIndex, tasks[thread + 1].startIndex);
                        for (IndexType i = 0; i < tasks[thread].numberOfBlocksWritten; ++i) {
                            //Write the next block to the front
                            if (bufferRange.left + i == bufferTargetIndex) {
                                //Nothing to do, there were no other blocks
                                bufferTargetIndex += tasks[thread].numberOfBlocksWritten;
                                break;
                            }
                            AssertMsg(bufferArray[bufferRange.left + i].size > 0, "Buffer block is not there.");
                            AssertMsg(bufferArray[bufferTargetIndex].size == 0, "Buffer target is not empty.");
                            //std::cout << "Copying block from " << (bufferRange.left + i) << " to " << bufferTargetIndex << std::endl;
                            bufferArray[bufferTargetIndex] = bufferArray[bufferRange.left + i];
                            bufferArray[bufferRange.left + i] = PmaEntryType{};
                            ++bufferTargetIndex;
                        }
                    }

                    //Adjust the buffer pointers
                    const Interval newBufferPointers(bufferPointerStart, bufferTargetIndex);
                    AssertMsg(newBufferPointers.length() <= bufferPointers[mergeBlock].length(), "Made buffer pointers larger");
                    if (newBufferPointers.length() == 0) {
                        bufferPointers[mergeBlock] = Interval(0,0);
                    } else {
                        bufferPointers[mergeBlock] = newBufferPointers;
                    }
                }
                //Reset everything as the merge group ends here.
                mergeBlock = -1;
                bufferTargetIndex = -1;
                bufferPointerStart = -1;
            }
        }
    }

    /**
     * Rebalances the portion of the rebalancing task taskIndex whose blocks
     * globally fall into the given block range.
     */
    PartialRebalancingTask getPartialRebalancingRange(IndexType taskIndex, const Interval &globalBlockRange) {
        //Range of rebalancing blocks that this task contains, seen globally.
        const Interval taskBlockRange(taskIndex > 0 ? prefixNumberOfRebalancingBlocks[taskIndex - 1] : 0, prefixNumberOfRebalancingBlocks[taskIndex]);
        //std::cout << "      Rebalancing global block range " << globalBlockRange << " of task " << taskIndex << " with global blocks " << taskBlockRange << std::endl;
        Interval globalBlockRangePart = taskBlockRange.cut(globalBlockRange);
        //std::cout << "      Interval intersection is " << globalBlockRangePart << std::endl;
        if (globalBlockRangePart.length() == 0) return PartialRebalancingTask();
        //Convert the subset globalBlockRangePart of taskBlockRange to a local interval (indices relative to the task only).
        const Interval localBlockRange(globalBlockRangePart.left - taskBlockRange.left, globalBlockRangePart.right - taskBlockRange.left);
        //std::cout << "      Converts to local block range " << localBlockRange << std::endl;

        //Find the start and end block with the required indices in the task
        const ExtendedPmaIndex blockStart = rebalancingManager.findBlockInRebalancingTask(rebalancingTasks[taskIndex].rootNode, localBlockRange.left);
        const ExtendedPmaIndex blockEnd = rebalancingManager.findBlockInRebalancingTask(rebalancingTasks[taskIndex].rootNode, localBlockRange.right);
        //std::cout << "      And extended blocks " << blockStart << " -- " << blockEnd << std::endl;
        AssertMsg(localBlockRange.left >= 0 && localBlockRange.left < rebalancingTasks[taskIndex].numberOfBlocks, "Local subrange start is invalid.");

        return PartialRebalancingTask{blockStart, blockEnd, localBlockRange.left};
    }

    SizeType startPartialRebalancingTask(IndexType taskIndex, const PartialRebalancingTask &task) {
        //std::cout << "   Starting partial rebalance on task " << taskIndex << ", partial task " << task << std::endl;
        return rebalanceBlocks(taskIndex, task.blockStart, task.blockEnd, task.outputOffset);
    }

    /**
     * Rebalances the entire rebalancing task at taskIndex.
     */
    SizeType startFullRebalancingTask(IndexType taskIndex) {
        //std::cout << "   Starting full rebalance on task " << taskIndex << std::endl;
        const SizeType effectivePmaSize = std::max(static_cast<SizeType>(1), size);
        Interval pmaRange = rebalancingTasks[taskIndex].pmaRange;
        if (pmaRange.left >= effectivePmaSize) {
            AssertMsg(rebalancingTasks[taskIndex].numberOfBlocks == 0, "Blocks might be lost");
            return 0;
        }
        if (pmaRange.right > effectivePmaSize) pmaRange.right = effectivePmaSize;
        if (pmaRange.length() == 0) {
            AssertMsg(rebalancingTasks[taskIndex].numberOfBlocks == 0, "Blocks might be lost");
            return 0;
        }

        //Rebalance all the blocks
        const ExtendedPmaIndex blockStart(pmaRange.left, -1);
        const ExtendedPmaIndex blockEnd(pmaRange.right, -1);
        return rebalanceBlocks(taskIndex, blockStart, blockEnd, 0);
    }

    /**
     * Writes all the blocks of the given rebalancing task from pma and buffer
     * array that are between blockStart and blockEnd to the task's output
     * range in the swapPma, offset by the given outputOffset.
     */
    SizeType rebalanceBlocks(IndexType taskIndex, const ExtendedPmaIndex &blockStart, const ExtendedPmaIndex &blockEnd, SizeType outputOffset) {
        rebalancingManager.clearRebalancedFlag(rebalancingTasks[taskIndex].rootNode);
        //std::cout << "      Rebalancing task " << taskIndex << " from " << blockStart << " to " << blockEnd << " with output offset " << outputOffset << std::endl;
        //Iterate over the blocks, skipping blocks that do not need to be rewritten and writing all blocks to the swapPma.
        ExtendedPmaIndex blockIndex = blockStart;
        const IndexType swapPmaOutputStart = rebalancingTasks[taskIndex].pmaRange.left + outputOffset;
        IndexType swapPmaOutputNumber = 0;
        while (blockIndex < blockEnd) {
            //Write all the blocks and buffer blocks of the current blockIndex to the swapPma.
            SizeType numberOfRemovedBlocks = 0;
            if (blockIndex.bufferIndex == -1) {
                if (!empty(blockIndex.pmaIndex)) {
                    //Write the block itself to the swapPma
                    const IndexType writeIndex = getDenseWriteIndex(swapPmaOutputStart, swapPmaOutputNumber);
                    AssertMsg(rebalancingTasks[taskIndex].pmaRange.contains(writeIndex), "Writing to an invalid index.");
                    swapPma[writeIndex] = pma[blockIndex.pmaIndex];
                    pma[blockIndex.pmaIndex] = PmaEntryType{};
                    ++numberOfRemovedBlocks;
                    ++swapPmaOutputNumber;
                }
                blockIndex.bufferIndex = 0;//Set the buffer index to the buffer start.
            }
            
            //Write the buffer blocks of this block to the output
            const IndexType bufferBlockStart = bufferPointers[blockIndex.pmaIndex].left + blockIndex.bufferIndex;
            IndexType bufferBlockEnd = bufferPointers[blockIndex.pmaIndex].right;
            if (blockIndex.pmaIndex == blockEnd.pmaIndex) {
                //The current pmaBlock is the last block, ensure that we do not write too many buffer blocks
                bufferBlockEnd = bufferPointers[blockEnd.pmaIndex].left + blockEnd.bufferIndex;
            }
            IndexType bufferBlockIndex;
            for (bufferBlockIndex = bufferBlockStart; bufferBlockIndex < bufferBlockEnd; ++bufferBlockIndex) {
                AssertMsg((bufferArray[bufferBlockIndex].size == 0) == (bufferArray[bufferBlockIndex].head == NULL_KEY), "Invalid buffer array block");
                AssertMsg((bufferArray[bufferBlockIndex].size == 0) == (bufferArray[bufferBlockIndex].block == nullptr), "Invalid buffer array block");
                if (bufferArray[bufferBlockIndex].size > 0) {
                    //Write the block to the swapPma
                    const IndexType writeIndex = getDenseWriteIndex(swapPmaOutputStart, swapPmaOutputNumber);
                    AssertMsg(rebalancingTasks[taskIndex].pmaRange.contains(writeIndex), "Writing to an invalid index.");
                    swapPma[writeIndex] = bufferArray[bufferBlockIndex];
                    //std::cout << "      Writing to swap index " << writeIndex << " for rebalancing node " << node << " (buffer block)" << std::endl;
                    bufferArray[bufferBlockIndex] = PmaEntryType{};
                    ++swapPmaOutputNumber;
                    ++numberOfRemovedBlocks;
#ifndef NDEBUG
                    ++bufferReads;
#endif
                }
            }
            ++blockIndex.pmaIndex;
            blockIndex.bufferIndex = -1;
        }
        //std::cout << "      Rebalance for task " << taskIndex << " wrote " << swapPmaOutputNumber << " blocks" << std::endl;
        return swapPmaOutputNumber;
    }

    void startPartialClearTask(const PartialRebalancingTask &task) {
        //std::cout << "   Starting partial rebalance on task " << taskIndex << ", partial task " << task << std::endl;
        clearBufferBlockPointers(task.blockStart, task.blockEnd);
    }

    void startFullClearTask(IndexType taskIndex) {
        //std::cout << "   Starting full rebalance on task " << taskIndex << std::endl;
        const SizeType effectivePmaSize = std::max(static_cast<SizeType>(1), size);
        Interval pmaRange = rebalancingTasks[taskIndex].pmaRange;
        if (pmaRange.left >= effectivePmaSize) {
            AssertMsg(rebalancingTasks[taskIndex].numberOfBlocks == 0, "Blocks might be lost");
            return;
        }
        if (pmaRange.right > effectivePmaSize) pmaRange.right = effectivePmaSize;
        if (pmaRange.length() == 0) {
            AssertMsg(rebalancingTasks[taskIndex].numberOfBlocks == 0, "Blocks might be lost");
            return;
        }

        //Rebalance all the blocks
        const ExtendedPmaIndex blockStart(pmaRange.left, -1);
        const ExtendedPmaIndex blockEnd(pmaRange.right, -1);
        clearBufferBlockPointers(blockStart, blockEnd);
    }

#if !USE_BUCKET_MAP
    template<typename T>
    void copyRecountingNodesToVector(T &recountingNodesToCopy) {
        recountingNodesVector.reserve(recountingNodesToCopy.size());
        for (const std::pair<IndexType, bool> &item : recountingNodesToCopy) {
            recountingNodesVector.emplace_back(item);
        }
        recountingNodesVector.resize(recountingNodesToCopy.size());
    }
#endif

    void clearBufferBlockPointers(const ExtendedPmaIndex &blockStart, const ExtendedPmaIndex &blockEnd) {
        ExtendedPmaIndex blockIndex = blockStart;
        while (blockIndex < blockEnd) {
            bufferPointers[blockIndex.pmaIndex] = Interval();
            ++blockIndex.pmaIndex;
        }
    }

    IndexType findRebalancingTask(IndexType blockNumber) const {
        const IndexType result = std::lower_bound(prefixNumberOfRebalancingBlocks.begin(), prefixNumberOfRebalancingBlocks.end(), blockNumber) - prefixNumberOfRebalancingBlocks.begin();
        //std::cout << "   Searching for block number " << blockNumber << ", result is " << result << std::endl;
        return result;
    }

    void startRebalancingTask(const RebalancingTask &task) {
        const SizeType effectivePmaSize = std::max(static_cast<SizeType>(1), size);
        Interval pmaRange = task.pmaRange;
        if (pmaRange.left >= effectivePmaSize) {
            AssertMsg(task.numberOfBlocks == 0, "Blocks might be lost");
            return;
        }
        if (pmaRange.right > effectivePmaSize) pmaRange.right = effectivePmaSize;
        if (pmaRange.length() == 0) {
            AssertMsg(task.numberOfBlocks == 0, "Blocks might be lost");
            return;
        }

        //std::cout << "   Rebalancing task " << task << std::endl;
        //Write the blocks from pmaRange into the swap array.
        IndexType swapPmaOutputNumber = 0;
        for (IndexType blockIndex = pmaRange.left; blockIndex < pmaRange.right; ++blockIndex) {
            SizeType numberOfRemovedBlocks = 0;
            if (!empty(blockIndex)) {
                //Write the block to the swapPma
                const IndexType writeIndex = getDenseWriteIndex(pmaRange.left, swapPmaOutputNumber);
                swapPma[writeIndex] = pma[blockIndex];
                //std::cout << "      Writing to swap index " << writeIndex << " for rebalancing node " << node << std::endl;
                pma[blockIndex] = PmaEntryType{};
                ++numberOfRemovedBlocks;
                ++swapPmaOutputNumber;
                AssertMsg(swapPmaOutputNumber <= task.numberOfBlocks, "Too many blocks written to swap pma");
            }
            //Write the buffer blocks of this block to the output
            for (IndexType bufferBlockIndex = bufferPointers[blockIndex].left; bufferBlockIndex < bufferPointers[blockIndex].right; ++bufferBlockIndex) {
                AssertMsg((bufferArray[bufferBlockIndex].size == 0) == (bufferArray[bufferBlockIndex].head == NULL_KEY), "Invalid buffer array block");
                AssertMsg((bufferArray[bufferBlockIndex].size == 0) == (bufferArray[bufferBlockIndex].block == nullptr), "Invalid buffer array block");
                if (bufferArray[bufferBlockIndex].size > 0) {
                    //Write the block to the swapPma
                    const IndexType writeIndex = getDenseWriteIndex(pmaRange.left, swapPmaOutputNumber);
                    swapPma[writeIndex] = bufferArray[bufferBlockIndex];
                    //std::cout << "      Writing to swap index " << writeIndex << " for rebalancing node " << node << " (buffer block)" << std::endl;
                    bufferArray[bufferBlockIndex] = PmaEntryType{};
                    ++swapPmaOutputNumber;
                    AssertMsg(swapPmaOutputNumber <= task.numberOfBlocks, "Too many blocks written to swap pma");
                    ++numberOfRemovedBlocks;
#ifndef NDEBUG
                    ++bufferReads;
#endif
                }
            }
            bufferPointers[blockIndex] = Interval(0,0);
        }
    }

    void startPartialRewritingTask(IndexType taskIndex, const Interval &globalBlockRange) {
        //Range of rebalancing blocks that this task contains, seen globally.
        AssertMsg((taskIndex > 0 ? taskIndex - 1 : 0) < static_cast<IndexType>(prefixNumberOfRebalancingBlocks.size()), "Invalid index");
        AssertMsg(taskIndex < static_cast<IndexType>(prefixNumberOfRebalancingBlocks.size()), "Invalid index");
        const Interval taskBlockRange(taskIndex > 0 ? prefixNumberOfRebalancingBlocks[taskIndex - 1] : 0, prefixNumberOfRebalancingBlocks[taskIndex]);
        Interval globalBlockRangePart = taskBlockRange.cut(globalBlockRange);
        if (globalBlockRangePart.length() == 0) return;
        //Convert the subset globalBlockRangePart of taskBlockRange to a local interval (indices relative to the task only).
        const Interval localBlockRange(globalBlockRangePart.left - taskBlockRange.left, globalBlockRangePart.right - taskBlockRange.left);

        //Get the input range in the swapPma (global indices)
        const IndexType swapPmaOffset = globalBlockRangePart.left - taskBlockRange.left;
        const SizeType numberOfBlocks = globalBlockRangePart.length();
        const IndexType swapPmaStart = rebalancingTasks[taskIndex].pmaRange.left + swapPmaOffset;
        const IndexType swapPmaEnd = rebalancingTasks[taskIndex].pmaRange.left + swapPmaOffset + numberOfBlocks;
        const Interval swapPmaRange(swapPmaStart, swapPmaEnd);
        //std::cout << "      Partial Task " << taskIndex << " Rewriting swap range " << swapPmaRange << std::endl;

        //Get the output range in the pma (may not be the same in this case!)
        const IndexType pmaStart = getUniformWriteIndex(rebalancingTasks[taskIndex].pmaRange.left, swapPmaOffset, rebalancingTasks[taskIndex].pmaRange.length(), rebalancingTasks[taskIndex].numberOfBlocks);
        const IndexType pmaEnd = getUniformWriteIndex(rebalancingTasks[taskIndex].pmaRange.left, swapPmaOffset + numberOfBlocks, rebalancingTasks[taskIndex].pmaRange.length(), rebalancingTasks[taskIndex].numberOfBlocks);
        const Interval pmaRange(pmaStart, pmaEnd);

        //Rewrite the blocks.
        rewriteBlocks(swapPmaRange, pmaRange, numberOfBlocks);
    }

    void startFullRewritingTask(IndexType taskIndex) {
        //std::cout << "      Full Task " << taskIndex << " Rewriting swap range " << rebalancingTasks[taskIndex].pmaRange << std::endl;
        rewriteBlocks(rebalancingTasks[taskIndex].pmaRange, rebalancingTasks[taskIndex].pmaRange, rebalancingTasks[taskIndex].numberOfBlocks);
    }

    /**
     * Uniformly distributes numberOfBlocks blocks from swapPmaRange of swapPma to the pmaRange of pma.
     */
    void rewriteBlocks(const Interval &swapPmaRange, const Interval &pmaRange, SizeType numberOfBlocks) {
        IndexType pmaOutputNumber = 0;
        AssertMsg(numberOfBlocks <= swapPmaRange.length(), "Not enough blocks in interval");
        for (IndexType blockIndex = swapPmaRange.left; blockIndex < swapPmaRange.left + numberOfBlocks; ++blockIndex) {
            AssertMsg(swapPma[blockIndex].size > 0, "Invalid block");
            //Write the block to the pma
            //std::cout << "      Reading from swap index " << blockIndex << std::endl;
            const IndexType writeIndex = getUniformWriteIndex(pmaRange.left, pmaOutputNumber, pmaRange.length(), numberOfBlocks);
            //Add the block to the list of recounting blocks
            addRecountingNode(writeIndex, false);
            pma[writeIndex] = swapPma[blockIndex];
            swapPma[blockIndex] = PmaEntryType{};
            ++pmaOutputNumber;
        }
    }

    //This is currently not used!
    void startRewritingTask(const RebalancingTask &task, SizeType pmaSize) {
        Interval pmaRange = task.pmaRange;
        if (pmaRange.left >= pmaSize) return;
        if (pmaRange.right > pmaSize) pmaRange.right = pmaSize;
        if (pmaRange.length() == 0) return;

        //std::cout << "   Rewriting task " << task << std::endl;

        IndexType pmaOutputNumber = 0;
        for (IndexType blockIndex = pmaRange.left; blockIndex < pmaRange.right; ++blockIndex) {
            if (swapPma[blockIndex].size > 0) {
                //Write the block to the pma
                //std::cout << "      Reading from swap index " << blockIndex << std::endl;
                const IndexType writeIndex = getUniformWriteIndex(pmaRange.left, pmaOutputNumber, pmaRange.length(), task.numberOfBlocks);
                addRecountingNode(writeIndex, false);
                pma[writeIndex] = swapPma[blockIndex];
                swapPma[blockIndex] = PmaEntryType{};
                ++pmaOutputNumber;
            }
        }
    }

    void addRecountingNode(IndexType blockIndex, bool needsRebalancing) {
        const IndexType recountingNodeIndex = rebalancingManager.lowestLevelNodeIndex(blockIndex);
#if USE_BUCKET_MAP
        recountingNodesMap.insert(std::make_pair(recountingNodeIndex, needsRebalancing));
#else
        RecountingNodeMap::iterator it;
        bool inserted;
        std::tie(it, inserted) = recountingNodesMap.insert(std::make_pair(recountingNodeIndex, needsRebalancing));
        if (!inserted && needsRebalancing && !it->second) {
            //Entry already there, but it needs to be rebalanced.
            it->second = true;
        }
#endif
    }

    IndexType getUniformWriteIndex(IndexType outputStart, IndexType blockNumber, SizeType outputLength, SizeType numberOfBlocks) {
        return outputStart + (blockNumber * outputLength) / numberOfBlocks;
    }

    void resizeMainPma(SizeType newSize) {
        size = newSize;
        if (swapCapacity != capacity) {
            pma = std::make_unique<PmaEntryType[]>(swapCapacity);
            bufferPointers = std::make_unique<Interval[]>(swapCapacity);
            if constexpr (Debug) std::cout << "Resized pma to " << swapCapacity << std::endl;
            capacity = swapCapacity;
        }
    }

    /**
     * Increases the size of the buffer array to the given new value.
     * If necessary, increases capacity and reallocates buffer array to the larger capacity.
     */
    void increaseBufferArraySize(IndexType newSize) {
        if (newSize >= bufferCapacity) {
            //Resize necessary.
            while (bufferCapacity <= newSize) {
                bufferCapacity *= 2;
            }
            bufferArray = std::make_unique<PmaEntryType[]>(bufferCapacity);
        }
    }

    /**
     * Increases the size of the swapPma to the given new value.
     * If necessary, increases capacity and reallocates the swap data structures to the larger capacity.
     */
    void increaseSwapPmaSize(IndexType newSize) {
        if (newSize >= swapCapacity) {
            //Resize necessary.
            while (swapCapacity <= newSize) {
                swapCapacity *= 2;
            }
            swapPma = std::make_unique<PmaEntryType[]>(swapCapacity);
        }
    }

    SizeType maxRedistributionWidth(SizeType numberOfElements, SizeType newElements) {
        //Assume only that in the end, all blocks will be at least half full.
        AssertMsg(numberOfElements >= 0 && newElements >= 0, "Negative numbers of elements.");
        const SizeType newNumberOfElements = numberOfElements + newElements;
        const SizeType newNumberOfBlocks = newNumberOfElements == 0 ? 0 : newNumberOfElements / MinBlockSize + 1;
        return newNumberOfBlocks;
    }

    void writeToBufferArray(const PmaEntryType &block, const Interval &outputRange, IndexType blockNumber, IndexType originBlock) {
        AssertMsg(!block.isEmpty(), "Writing a null block.");
        AssertMsg(block.size <= BlockSize && block.size >= MinBlockSize, "Block has invalid size.");
        const IndexType writeIndex = getDenseWriteIndex(outputRange.left, blockNumber);
        AssertMsg(writeIndex < outputRange.right, "Writing outside of the output range.");
        AssertMsg(writeIndex < bufferCapacity, "Writing to an invalid index");
        AssertMsg(bufferArray[writeIndex].block == nullptr, "Overwriting a block");
        bufferArray[writeIndex] = block;
        addRecountingNode(originBlock, true);
#ifndef NDEBUG
        ++bufferWrites;
#endif
    }

    IndexType getDenseWriteIndex(IndexType outputStart, IndexType blockNumber) const {
        return outputStart + blockNumber;
    }

    bool empty(IndexType index) const {
        if (index >= size) return true;
        AssertMsg((head(index) == NULL_KEY) == pma[index].isEmpty(), "Cell " << index << " has invalid state.");
        AssertMsg((head(index) == NULL_KEY) == (blockSize(index) == 0), "Cell " << index << " has invalid state.");
        return head(index) == NULL_KEY;
    }

    /**
     * This is currently not used. Can be used as part of an interface that does not require presorted,
     * duplicate-free batches.
     */
    void deduplicate(Pointer &batch, SizeType &batchSize) {
        //Copy the batch to keep it in its previous shape if it is ever reused.
        Pointer newBatch = static_cast<Pointer>(malloc(static_cast<size_t>(batchSize) * sizeof(ValueType)));
        std::copy(batch, batch + batchSize, newBatch);
        auto newEnd = std::unique(newBatch, newBatch + batchSize, [](const ValueType &lhs, const ValueType &rhs) -> bool {
            return KeyOf::keyOfValue(lhs) == KeyOf::keyOfValue(rhs);
        });
        batch = newBatch;
        batchSize = static_cast<SizeType>(newEnd - batch);
    }

public:
    iterator begin() {
        IndexType first = 0;
        while (empty(first) && first != size) ++first;
        return iterator(this, first, 0);
    }

    iterator end() {
        return iterator(this, size, 0);
    }

    [[nodiscard]] KeyType& head(IndexType index) {
        return pma[index].head;
    }

    [[nodiscard]] KeyType head(IndexType index) const {
        return pma[index].head;
    }

    [[nodiscard]] BlockSizeType& blockSize(IndexType index) {
        return pma[index].size;
    }

    [[nodiscard]] BlockSizeType blockSize(IndexType index) const {
        return pma[index].size;
    }

public:
    // For debugging only.
    void printStats() {
        std::cout << "Batched sparse array with"
            << " capacity " << capacity
            << ", number of elements " << numberOfElements()
            << ", active region [" << 0 << ", " << size << ")"
            << ", is valid? " << isValid()
            << ", number of blocks " << numberOfBlocks()
            << ", density " << (static_cast<double>(numberOfBlocks()) / static_cast<double>(size))
            << std::endl;
    }

    SizeType numberOfElements() const {
        IndexType n = 0;
        for (IndexType i = 0; i < capacity; ++i) {
            if (!empty(i)) {
                n += blockSize(i);
            }
        }
        return n;
    }

    SizeType numberOfBlocks() const {
        IndexType nBlocks = 0;
        for (IndexType i = 0; i < capacity; ++i) {
            if (!empty(i)) {
                ++nBlocks;
            }
        }
        return nBlocks;
    }

    void printContent() const {
        for (IndexType i = 0; i < capacity; ++i) {
            if (!empty(i)) {
                std::cout << "Block " << i << ": " << pma[i] << std::endl;
            }
        }
        std::cout << "Size = " << size << std::endl;
    }

    void printBufferContent() const {
        for (IndexType i = 0; i < capacity; ++i) {
            if (bufferArray[i].size != 0) {
                std::cout << "Buffer Block " << i << ": " << bufferArray[i] << std::endl;
            }
        }
    }

    void printBlockStructure() const {
        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        for (IndexType i = 0; i < size; ++i) {
            if (!empty(i)) {
                std::cout << "X";
            } else {
                std::cout << "_";
            }
        }
        std::cout << std::endl;
        std::cout << "Density: " << (static_cast<double>(numberOfBlocks()) / static_cast<double>(size)) << std::endl;
    }

    void printBlockHistogram() const {
        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        const SizeType numberOfBuckets = 35;
        const SizeType blocksPerBucket = size / numberOfBuckets + 1;
        const SizeType fullWidth = 100;
        for (IndexType bucket = 0; bucket < numberOfBuckets; ++bucket) {
            SizeType numberOfBlocks = 0;
            for (IndexType i = bucket * blocksPerBucket; i < std::min(size, (bucket + 1) * blocksPerBucket); ++i) {
                numberOfBlocks += empty(i) ? 0 : 1;
            }
            //Output
            const SizeType width = fullWidth * numberOfBlocks / blocksPerBucket;
            for (IndexType i = 0; i < width; ++i) {
                std::cout << "*";
            }
            for (IndexType i = width; i < fullWidth; ++i) {
                std::cout << " ";
            }
            std::cout << "|" << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Density: " << (static_cast<double>(numberOfBlocks()) / static_cast<double>(size)) << std::endl;
    }

    void printHeads() const {
        for (IndexType i = 0; i < size; ++i) {
            std::cout << head(i) << " ";
        }
        std::cout << std::endl;
    }

    void print() {
        printStats();
        printContent();
        isValid() ? std::cout << "Valid." << std::endl : std::cout << " ERROR: INVALID structure, not sorted correctly." << std::endl;
    }

    bool __attribute__ ((noinline)) isValid() const {
        KeyType currentHead = NULL_KEY;
        KeyType currentElement = NULL_KEY;
        for (IndexType i = 0; i < capacity; ++i) {
            if (!empty(i)) {
                AssertMsg(pma[i].size <= BlockSize, "Block too large.");
                AssertMsg(pma[i].size >= MinBlockSize, "Block too small.");
                if (pma[i].size > BlockSize || pma[i].size < MinBlockSize) return false;
                if (comp(head(i), currentHead) || head(i) == currentHead) {
                    std::cout << "Head " << head(i) << " at " << i << " larger than previous " << currentHead << std::endl;
                    AssertMsg(false, "Not sorted correctly");
                    return false;//Not ordered or duplicate
                }
                currentHead = head(i);
                if (head(i) != KeyOf::keyOfValue(pma[i].get(0))) {
                    AssertMsg(false, "Wrong head");
                    std::cout << "Head " << head(i) << " != block start " << KeyOf::keyOfValue(pma[i].get(0)) << std::endl;
                    return false;
                }
                for (BlockIndexType j = 0; j < blockSize(i); ++j) {
                    KeyType element = KeyOf::keyOfValue(pma[i].get(j));
                    if (comp(element, currentElement) || element == currentElement) {
                        std::cout << "Block " << i << " not sorted correctly at element " << element << ", previous is " << currentElement << std::endl;
                        AssertMsg(false, "Not sorted correctly.");
                        return false;
                    }
                    currentElement = element;
                }
            } else {
                AssertMsg(pma[i].isEmpty() && blockSize(i) == 0, "Wrong pma values");
                if (!pma[i].isEmpty() || blockSize(i) != 0) return false;
            }
            for (IndexType bufferIndex = bufferPointers[i].left; bufferIndex < bufferPointers[i].right; ++bufferIndex) {
                if (bufferArray[bufferIndex].size != 0) {
                    //AssertMsg(bufferArray[bufferIndex].size <= BlockSize, "Block too large.");
                    //AssertMsg(bufferArray[bufferIndex].size >= MinBlockSize, "Block too small.");
                    if (bufferArray[bufferIndex].size > BlockSize || bufferArray[bufferIndex].size < MinBlockSize) return false;
                    if (comp(bufferArray[bufferIndex].head, currentHead) || bufferArray[bufferIndex].head == currentHead) {
                        std::cout << "Head at buffer larger than previous " << currentHead << std::endl;
                        AssertMsg(false, "Not sorted correctly");
                        return false;//Not ordered or duplicate
                    }
                    currentHead = bufferArray[bufferIndex].head;
                    if (bufferArray[bufferIndex].head != KeyOf::keyOfValue(bufferArray[bufferIndex].get(0))) {
                        AssertMsg(false, "Wrong head");
                        std::cout << "Head of buffer block != block start " << KeyOf::keyOfValue(bufferArray[bufferIndex].get(0)) << std::endl;
                        return false;
                    }
                    for (BlockIndexType j = 0; j < bufferArray[bufferIndex].size; ++j) {
                        KeyType element = KeyOf::keyOfValue(bufferArray[bufferIndex].get(j));
                        if (comp(element, currentElement) || element == currentElement) {
                            std::cout << "Buffer block not sorted correctly at element " << element << ", previous is " << currentElement << std::endl;
                            AssertMsg(false, "Not sorted correctly.");
                            return false;
                        }
                        currentElement = element;
                    }
                } else {
                    AssertMsg(bufferArray[bufferIndex].isEmpty() && bufferArray[bufferIndex].size == 0, "Wrong buffer array values");
                    if (!bufferArray[bufferIndex].isEmpty() || bufferArray[bufferIndex].size != 0) return false;
                }
            }

        }
        return true;
    }

    bool __attribute__ ((noinline)) validateSwapPma() const {
        KeyType currentHead = NULL_KEY;
        KeyType currentElement = NULL_KEY;
        for (IndexType i = 0; i < capacity; ++i) {
            if (swapPma[i].block != nullptr) {
                if (swapPma[i].head <= currentHead) {
                    AssertMsg(false, "not ordered");
                    std::cout << "Head " << swapPma[i].head << " at " << i << " larger than previous " << currentHead << std::endl;
                    return false; // Not ordered or duplicate
                }
                currentHead = swapPma[i].head;
                if (swapPma[i].head != KeyOf::keyOfValue(swapPma[i].get(0))) {
                    AssertMsg(false, "Wrong head");
                    return false;
                }
                for (BlockIndexType j = 0; j < swapPma[i].size; ++j) {
                    KeyType element = KeyOf::keyOfValue(swapPma[i].get(j));
                    if (element <= currentElement) {
                        AssertMsg(false, "not sorted correctly.");
                        return false;
                    }
                    currentElement = element;
                }
            } else {
                AssertMsg(swapPma[i].block == nullptr && swapPma[i].size == 0, "Wrong pma values");
                if (swapPma[i].block != nullptr || swapPma[i].size != 0) return false;
            }
        }
        return true;
    }

    void validateTaskDistribution(SizeType batchSize, bool isInPlace) const {
        AssertMsg(tasks[0].firstBatchElement == 0, "Tasks 0 does not start at batch start");

        for (UnsignedIndexType thread = 0; thread <= numberOfThreads; ++thread) {
            if (!isInPlace && tasks[thread].type == TaskType::INVERSE_MERGE) {
                AssertMsg(tasks[thread].mergedTasks < numberOfThreads, "Invalid number of merged tasks");
                AssertMsg(tasks[thread].mergedTaskIndex < tasks[thread].mergedTasks, "Wrong merged task index");
            }
        }
        AssertMsg(tasks[numberOfThreads].firstBatchElement == batchSize, "Sentinel task does not start at batch start");
        AssertMsg(tasks[numberOfThreads].blockForBatchSplitter == size, "Sentinel task does not end at sds end");
    }

    bool batchSorted(Pointer batch, SizeType batchSize) const {
        KeyType last = NULL_KEY;
        for (IndexType i = 0; i < batchSize; ++i) {
            if (comp(KeyOf::keyOfValue(batch[i]), last))
                return false;
            last = KeyOf::keyOfValue(batch[i]);
        }
        return true;
    }

    static const std::string type() {
        if constexpr (SplitRebalancingTasks) {
            if constexpr (Profile || ProfileConcurrent) {
                return "BBPMA-b=" + std::to_string(BlockSize) + "-s=" + std::to_string(SegmentWidth) + "-d=" + getDensityId() + "-profiled";
            } else {
                return "BBPMA-b=" + std::to_string(BlockSize) + "-s=" + std::to_string(SegmentWidth) + "-d=" + getDensityId();
            }
        } else {
            if constexpr (Profile || ProfileConcurrent) {
                return "BBPMA-unsplit-b=" + std::to_string(BlockSize) + "-s=" + std::to_string(SegmentWidth) + "-d=" + getDensityId() + "-profiled";
            } else {
                return "BBPMA-unsplit-b=" + std::to_string(BlockSize) + "-s=" + std::to_string(SegmentWidth) + "-d=" + getDensityId();
            }
        }
    }

    static const std::string getDensityId() {
        return DensityManager::type();
    }

    BlockSizeType getBlockSize() const {
        return BlockSize;
    }

    BlockSizeType getSegmentSize() const {
        return SegmentWidth;
    }

    SizeType numberOfBufferBlocks(IndexType index) const {
        if (index >= capacity) return 0;
        return bufferPointers[index].length();
    }

    void printBufferStructure(SizeType currentBufferSize) {
        for (IndexType i = 0; i < capacity; ++i) {
            if (!empty(i)) {
                std::cout << "X: " << numberOfBufferBlocks(i) << " ";
            } else {
                std::cout << "_: " << numberOfBufferBlocks(i) << " ";
            }
            for (IndexType j = bufferPointers[i].left; j < std::min(currentBufferSize, bufferPointers[i].right); ++j) {
                if (!bufferArray[j].isEmpty()) {
                    std::cout << "B";
                } else {
                    std::cout << "-";
                }
            }
            std::cout << std::endl;
        }
    }

    void printBufferStructureSmall(SizeType newSize) {
        for (IndexType i = 0; i < newSize; ++i) {
            if (i < size && !empty(i)) {
                std::cout << "X";
            } else {
                std::cout << "_";
            }
            if (numberOfBufferBlocks(i) > 0) {
                std::cout << "(" << numberOfBufferBlocks(i) << ") ";
            }
        }
        std::cout << std::endl << std::endl << std::endl << std::endl;
    }

    void printRebalancingNodes(UnsignedIndexType thread) const {
        for (const RebalancingNode &node : tasks[thread].rebalancingNodes) {
            std::cout << "   " << node << std::endl;
        }
    }
};
}
