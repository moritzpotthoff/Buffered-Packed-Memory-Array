#include "batchedSparseArray.hpp"
#include "thread_coordination.hpp"
#include "definitions.hpp"
#include "inputGenerator.hpp"
#include "benchmarks.hpp"
#include "configuration.hpp"
#include "densityManager.hpp"

#define _MSC_VER false
#define TBB_ALLOCATOR_TRAITS_BROKEN false
#include "tbb/scalable_allocator.h"

#include <iostream>
#include <string>

using AllocatorType = tbb::scalable_allocator<Benchmark::DataType>;

//Definition of all the variants
using SearchDataStructureDensityDensityOneSegment256Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment256Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment256Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment256Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment256Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment256Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment256Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment256Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment256Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 256, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityOneSegment512Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 512, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment512Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 512, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment512Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 512, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment512Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 512, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment512Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 512, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment512Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 512, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment512Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 512, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment512Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 512, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment512Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 512, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityOneSegment1024Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment1024Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment1024Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment1024Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment1024Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment1024Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment1024Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment1024Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment1024Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 1024, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityOneSegment2048Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 2048, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment2048Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 2048, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment2048Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 2048, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment2048Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 2048, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment2048Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 2048, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment2048Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 2048, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment2048Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 2048, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment2048Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 2048, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment2048Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 2048, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityOneSegment4096Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 4096, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment4096Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 4096, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment4096Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 4096, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment4096Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 4096, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment4096Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 4096, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment4096Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 4096, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment4096Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 4096, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment4096Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 4096, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityOneSegment4096Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 4096, DataStructures::DensityOne, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityTwoSegment256Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 256, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment256Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 256, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment256Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 256, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment256Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 256, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment256Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 256, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment256Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 256, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment256Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 256, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment256Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 256, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment256Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 256, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityTwoSegment512Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 512, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment512Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 512, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment512Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 512, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment512Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 512, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment512Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 512, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment512Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 512, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment512Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 512, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment512Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 512, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment512Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 512, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityTwoSegment1024Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 1024, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment1024Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 1024, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment1024Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 1024, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment1024Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 1024, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment1024Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 1024, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment1024Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 1024, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment1024Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 1024, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment1024Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 1024, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment1024Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 1024, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityTwoSegment2048Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 2048, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment2048Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 2048, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment2048Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 2048, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment2048Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 2048, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment2048Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 2048, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment2048Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 2048, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment2048Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 2048, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment2048Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 2048, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment2048Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 2048, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityTwoSegment4096Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 4096, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment4096Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 4096, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment4096Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 4096, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment4096Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 4096, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment4096Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 4096, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment4096Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 4096, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment4096Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 4096, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment4096Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 4096, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityTwoSegment4096Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 4096, DataStructures::DensityTwo, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityThreeSegment256Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 256, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment256Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 256, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment256Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 256, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment256Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 256, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment256Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 256, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment256Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 256, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment256Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 256, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment256Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 256, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment256Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 256, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityThreeSegment512Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 512, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment512Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 512, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment512Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 512, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment512Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 512, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment512Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 512, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment512Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 512, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment512Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 512, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment512Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 512, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment512Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 512, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityThreeSegment1024Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 1024, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment1024Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 1024, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment1024Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 1024, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment1024Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 1024, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment1024Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 1024, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment1024Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 1024, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment1024Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 1024, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment1024Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 1024, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment1024Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 1024, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityThreeSegment2048Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 2048, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment2048Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 2048, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment2048Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 2048, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment2048Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 2048, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment2048Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 2048, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment2048Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 2048, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment2048Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 2048, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment2048Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 2048, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment2048Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 2048, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;

using SearchDataStructureDensityDensityThreeSegment4096Block16 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 16, 4096, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment4096Block32 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 32, 4096, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment4096Block64 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 64, 4096, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment4096Block128 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 128, 4096, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment4096Block256 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 256, 4096, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment4096Block512 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 512, 4096, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment4096Block1024 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 1024, 4096, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment4096Block2048 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 2048, 4096, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
using SearchDataStructureDensityDensityThreeSegment4096Block4096 = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, 4096, 4096, DataStructures::DensityThree, Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;
static const uint64_t MinKey = 1;

struct Query {
    Benchmark::DataType start;
    Benchmark::DataType end;
};

size_t numberOfQueries = 1000;
std::vector<Query> queries;

size_t numberOfSearchQueries = 1000;
std::vector<Benchmark::DataType> searchQueries;

template<typename T>
void runBenchmark(T &sds, Helpers::Configuration<Benchmark::DataType> config, int i, auto &inputGen) {
    //Running insertions
    Benchmark::BatchBenchmark<T> runner{};
    runner.run(std::ref(sds), std::ref(inputGen.prefillOps), std::ref(inputGen.ops), config.numThreads);

    /*
    std::cout << "RESULT" << " " << config << " iteration=" << i
              << " metric=prefillTime"
              << " sdsType=" << sds.type()
              << " benchmark=batchBlockSizeRandomDistribution"
              << " blockSize=" << sds.getBlockSize()
              << " segmentSize=" << sds.getSegmentSize()
              << " densityControls=" << sds.getDensityId()
              << " numberOfElements=" << sds.numberOfElements()
              << " value=" << std::fixed << std::setprecision(3) << std::chrono::duration<double>(runner.getDuration("prefill")).count()
              << std::endl;
    */
    std::cout << "RESULT"
            << " benchmark=parameterTuning"
            << " sdsType=" << sds.type()
            << " segmentSize=" << sds.getSegmentSize()
            << " blockSize=" << sds.getBlockSize()
            << " densityControls=" << sds.getDensityId()
            << " " << config
            << " iteration=" << i
            << " metric=workTime"
            << " numberOfElements=" << sds.numberOfElements()
            << " value=" << std::fixed << std::setprecision(3) << std::chrono::duration<double>(runner.getDuration("work")).count()
            << std::endl;

    //Running scan queries
    Helpers::Timer timer{};
    const size_t shortQueryMax = 1000;
    size_t shortQueryTime = 0;
    size_t shortQueryLength = 0;
    size_t longQueryTime = 0;
    size_t longQueryLength = 0;
    for (const Query &query : queries) {
        size_t queryLength = 0;
        size_t querySum = 0;
        timer.restart();
        sds.rangeQuery(query.start, query.end, [&queryLength, &querySum](const Benchmark::DataType &key) {
            ++queryLength;
            querySum += key;
        });//TODO add function and evaluation
        const size_t queryTime = timer.getMicroseconds();

        if (queryLength <= shortQueryMax) {
            shortQueryLength += queryLength;
            shortQueryTime += queryTime;
        } else {
            longQueryLength += queryLength;
            longQueryTime += queryTime;
        }
    }

    std::cout << "RESULT"
              << " benchmark=parameterTuning"
              << " sdsType=" << sds.type()
              << " segmentSize=" << sds.getSegmentSize()
              << " blockSize=" << sds.getBlockSize()
              << " densityControls=" << sds.getDensityId()
              << " " << config
              << " iteration=" << i
              << " numberOfElements=" << sds.numberOfElements()
              << " metric=shortQueries"
              << " unit=microseconds totalTime=" << shortQueryTime
              << " totalElements=" << shortQueryLength
              << std::endl;
    std::cout << "RESULT"
              << " benchmark=parameterTuning"
              << " sdsType=" << sds.type()
              << " segmentSize=" << sds.getSegmentSize()
              << " blockSize=" << sds.getBlockSize()
              << " densityControls=" << sds.getDensityId()
              << " " << config
              << " iteration=" << i
              << " numberOfElements=" << sds.numberOfElements()
              << " metric=longQueries"
              << " unit=microseconds totalTime=" << longQueryTime
              << " totalElements=" << longQueryLength
              << std::endl;

    //Running search queries
    size_t totalSearchTime = 0;
    timer.restart();
    for (const Benchmark::DataType key : searchQueries) {
        bool found;
        asm volatile("" : : "g"(found) : "memory");
        found = sds.contains(key);
        asm volatile("" : : : "memory");
    }
    totalSearchTime += timer.getMicroseconds();

    std::cout << "RESULT"
              << " benchmark=parameterTuning"
              << " sdsType=" << sds.type()
              << " segmentSize=" << sds.getSegmentSize()
              << " blockSize=" << sds.getBlockSize()
              << " densityControls=" << sds.getDensityId()
              << " " << config
              << " iteration=" << i
              << " numberOfElements=" << sds.numberOfElements()
              << " metric=searchQuery"
              << " unit=microseconds totalTime=" << totalSearchTime
              << " numberOfSearchQueries=" << numberOfSearchQueries
              << std::endl;
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    Helpers::Configuration<Benchmark::DataType> config(argc, argv);
    int i = 0;
    std::cout << "Running a benchmark with configuration: " << config << std::endl;
    Benchmark::RandomEngine rng(config.seed);
    Helpers::InputGenerator::BatchGenerator<Benchmark::DataType> inputGen(config, rng);
    std::cout << "Generated insertions." << std::endl;

    Helpers::UniformDistribution startDistribution(config.minKey, config.maxKey);
    Helpers::ZipfDistribution keyDifferenceDistribution(config.minKey, config.maxKey);
    queries.resize(numberOfQueries);
    for (Query &query : queries) {
        query.start = startDistribution(rng);
        query.end = query.start + keyDifferenceDistribution(rng);//TODO overflows?
    }
    searchQueries.resize(numberOfSearchQueries);
    for (size_t k = 0; k < numberOfSearchQueries; ++k) {
        searchQueries[k] = startDistribution(rng);
    }
    std::cout << "Generated queries." << std::endl;


    std::cout << "Running benchmarks" << std::endl;
    for (i = 0; i < config.iterations; ++i) {
        auto blockRunner = [config, i, &inputGen](auto&&... sds) {
            (runBenchmark(sds, config, i, inputGen), ...);
        };

        std::cout << "Running iteration " << i << std::endl;
        AllocatorType alloc;

        SearchDataStructureDensityDensityOneSegment256Block16 sdsDensityDensityOneSegment256Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment256Block32 sdsDensityDensityOneSegment256Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment256Block64 sdsDensityDensityOneSegment256Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment256Block128 sdsDensityDensityOneSegment256Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment256Block256 sdsDensityDensityOneSegment256Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment256Block512 sdsDensityDensityOneSegment256Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment256Block1024 sdsDensityDensityOneSegment256Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment256Block2048 sdsDensityDensityOneSegment256Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment256Block4096 sdsDensityDensityOneSegment256Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityOneSegment512Block16 sdsDensityDensityOneSegment512Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment512Block32 sdsDensityDensityOneSegment512Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment512Block64 sdsDensityDensityOneSegment512Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment512Block128 sdsDensityDensityOneSegment512Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment512Block256 sdsDensityDensityOneSegment512Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment512Block512 sdsDensityDensityOneSegment512Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment512Block1024 sdsDensityDensityOneSegment512Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment512Block2048 sdsDensityDensityOneSegment512Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment512Block4096 sdsDensityDensityOneSegment512Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityOneSegment1024Block16 sdsDensityDensityOneSegment1024Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment1024Block32 sdsDensityDensityOneSegment1024Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment1024Block64 sdsDensityDensityOneSegment1024Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment1024Block128 sdsDensityDensityOneSegment1024Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment1024Block256 sdsDensityDensityOneSegment1024Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment1024Block512 sdsDensityDensityOneSegment1024Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment1024Block1024 sdsDensityDensityOneSegment1024Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment1024Block2048 sdsDensityDensityOneSegment1024Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment1024Block4096 sdsDensityDensityOneSegment1024Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityOneSegment2048Block16 sdsDensityDensityOneSegment2048Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment2048Block32 sdsDensityDensityOneSegment2048Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment2048Block64 sdsDensityDensityOneSegment2048Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment2048Block128 sdsDensityDensityOneSegment2048Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment2048Block256 sdsDensityDensityOneSegment2048Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment2048Block512 sdsDensityDensityOneSegment2048Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment2048Block1024 sdsDensityDensityOneSegment2048Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment2048Block2048 sdsDensityDensityOneSegment2048Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment2048Block4096 sdsDensityDensityOneSegment2048Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityOneSegment4096Block16 sdsDensityDensityOneSegment4096Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment4096Block32 sdsDensityDensityOneSegment4096Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment4096Block64 sdsDensityDensityOneSegment4096Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment4096Block128 sdsDensityDensityOneSegment4096Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment4096Block256 sdsDensityDensityOneSegment4096Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment4096Block512 sdsDensityDensityOneSegment4096Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment4096Block1024 sdsDensityDensityOneSegment4096Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment4096Block2048 sdsDensityDensityOneSegment4096Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityOneSegment4096Block4096 sdsDensityDensityOneSegment4096Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityTwoSegment256Block16 sdsDensityDensityTwoSegment256Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment256Block32 sdsDensityDensityTwoSegment256Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment256Block64 sdsDensityDensityTwoSegment256Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment256Block128 sdsDensityDensityTwoSegment256Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment256Block256 sdsDensityDensityTwoSegment256Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment256Block512 sdsDensityDensityTwoSegment256Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment256Block1024 sdsDensityDensityTwoSegment256Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment256Block2048 sdsDensityDensityTwoSegment256Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment256Block4096 sdsDensityDensityTwoSegment256Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityTwoSegment512Block16 sdsDensityDensityTwoSegment512Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment512Block32 sdsDensityDensityTwoSegment512Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment512Block64 sdsDensityDensityTwoSegment512Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment512Block128 sdsDensityDensityTwoSegment512Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment512Block256 sdsDensityDensityTwoSegment512Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment512Block512 sdsDensityDensityTwoSegment512Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment512Block1024 sdsDensityDensityTwoSegment512Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment512Block2048 sdsDensityDensityTwoSegment512Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment512Block4096 sdsDensityDensityTwoSegment512Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityTwoSegment1024Block16 sdsDensityDensityTwoSegment1024Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment1024Block32 sdsDensityDensityTwoSegment1024Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment1024Block64 sdsDensityDensityTwoSegment1024Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment1024Block128 sdsDensityDensityTwoSegment1024Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment1024Block256 sdsDensityDensityTwoSegment1024Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment1024Block512 sdsDensityDensityTwoSegment1024Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment1024Block1024 sdsDensityDensityTwoSegment1024Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment1024Block2048 sdsDensityDensityTwoSegment1024Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment1024Block4096 sdsDensityDensityTwoSegment1024Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityTwoSegment2048Block16 sdsDensityDensityTwoSegment2048Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment2048Block32 sdsDensityDensityTwoSegment2048Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment2048Block64 sdsDensityDensityTwoSegment2048Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment2048Block128 sdsDensityDensityTwoSegment2048Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment2048Block256 sdsDensityDensityTwoSegment2048Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment2048Block512 sdsDensityDensityTwoSegment2048Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment2048Block1024 sdsDensityDensityTwoSegment2048Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment2048Block2048 sdsDensityDensityTwoSegment2048Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment2048Block4096 sdsDensityDensityTwoSegment2048Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityTwoSegment4096Block16 sdsDensityDensityTwoSegment4096Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment4096Block32 sdsDensityDensityTwoSegment4096Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment4096Block64 sdsDensityDensityTwoSegment4096Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment4096Block128 sdsDensityDensityTwoSegment4096Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment4096Block256 sdsDensityDensityTwoSegment4096Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment4096Block512 sdsDensityDensityTwoSegment4096Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment4096Block1024 sdsDensityDensityTwoSegment4096Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment4096Block2048 sdsDensityDensityTwoSegment4096Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityTwoSegment4096Block4096 sdsDensityDensityTwoSegment4096Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityThreeSegment256Block16 sdsDensityDensityThreeSegment256Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment256Block32 sdsDensityDensityThreeSegment256Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment256Block64 sdsDensityDensityThreeSegment256Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment256Block128 sdsDensityDensityThreeSegment256Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment256Block256 sdsDensityDensityThreeSegment256Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment256Block512 sdsDensityDensityThreeSegment256Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment256Block1024 sdsDensityDensityThreeSegment256Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment256Block2048 sdsDensityDensityThreeSegment256Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment256Block4096 sdsDensityDensityThreeSegment256Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityThreeSegment512Block16 sdsDensityDensityThreeSegment512Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment512Block32 sdsDensityDensityThreeSegment512Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment512Block64 sdsDensityDensityThreeSegment512Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment512Block128 sdsDensityDensityThreeSegment512Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment512Block256 sdsDensityDensityThreeSegment512Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment512Block512 sdsDensityDensityThreeSegment512Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment512Block1024 sdsDensityDensityThreeSegment512Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment512Block2048 sdsDensityDensityThreeSegment512Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment512Block4096 sdsDensityDensityThreeSegment512Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityThreeSegment1024Block16 sdsDensityDensityThreeSegment1024Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment1024Block32 sdsDensityDensityThreeSegment1024Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment1024Block64 sdsDensityDensityThreeSegment1024Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment1024Block128 sdsDensityDensityThreeSegment1024Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment1024Block256 sdsDensityDensityThreeSegment1024Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment1024Block512 sdsDensityDensityThreeSegment1024Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment1024Block1024 sdsDensityDensityThreeSegment1024Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment1024Block2048 sdsDensityDensityThreeSegment1024Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment1024Block4096 sdsDensityDensityThreeSegment1024Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityThreeSegment2048Block16 sdsDensityDensityThreeSegment2048Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment2048Block32 sdsDensityDensityThreeSegment2048Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment2048Block64 sdsDensityDensityThreeSegment2048Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment2048Block128 sdsDensityDensityThreeSegment2048Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment2048Block256 sdsDensityDensityThreeSegment2048Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment2048Block512 sdsDensityDensityThreeSegment2048Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment2048Block1024 sdsDensityDensityThreeSegment2048Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment2048Block2048 sdsDensityDensityThreeSegment2048Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment2048Block4096 sdsDensityDensityThreeSegment2048Block4096{alloc, config.numThreads};

        SearchDataStructureDensityDensityThreeSegment4096Block16 sdsDensityDensityThreeSegment4096Block16{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment4096Block32 sdsDensityDensityThreeSegment4096Block32{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment4096Block64 sdsDensityDensityThreeSegment4096Block64{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment4096Block128 sdsDensityDensityThreeSegment4096Block128{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment4096Block256 sdsDensityDensityThreeSegment4096Block256{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment4096Block512 sdsDensityDensityThreeSegment4096Block512{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment4096Block1024 sdsDensityDensityThreeSegment4096Block1024{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment4096Block2048 sdsDensityDensityThreeSegment4096Block2048{alloc, config.numThreads};
        SearchDataStructureDensityDensityThreeSegment4096Block4096 sdsDensityDensityThreeSegment4096Block4096{alloc, config.numThreads};
        
        blockRunner(
                sdsDensityDensityOneSegment256Block16,
                sdsDensityDensityOneSegment256Block32,
                sdsDensityDensityOneSegment256Block64,
                sdsDensityDensityOneSegment256Block128,
                sdsDensityDensityOneSegment256Block256,
                sdsDensityDensityOneSegment256Block512,
                sdsDensityDensityOneSegment256Block1024,
                sdsDensityDensityOneSegment256Block2048,
                sdsDensityDensityOneSegment256Block4096,

                sdsDensityDensityOneSegment512Block16,
                sdsDensityDensityOneSegment512Block32,
                sdsDensityDensityOneSegment512Block64,
                sdsDensityDensityOneSegment512Block128,
                sdsDensityDensityOneSegment512Block256,
                sdsDensityDensityOneSegment512Block512,
                sdsDensityDensityOneSegment512Block1024,
                sdsDensityDensityOneSegment512Block2048,
                sdsDensityDensityOneSegment512Block4096,

                sdsDensityDensityOneSegment1024Block16,
                sdsDensityDensityOneSegment1024Block32,
                sdsDensityDensityOneSegment1024Block64,
                sdsDensityDensityOneSegment1024Block128,
                sdsDensityDensityOneSegment1024Block256,
                sdsDensityDensityOneSegment1024Block512,
                sdsDensityDensityOneSegment1024Block1024,
                sdsDensityDensityOneSegment1024Block2048,
                sdsDensityDensityOneSegment1024Block4096,

                sdsDensityDensityOneSegment2048Block16,
                sdsDensityDensityOneSegment2048Block32,
                sdsDensityDensityOneSegment2048Block64,
                sdsDensityDensityOneSegment2048Block128,
                sdsDensityDensityOneSegment2048Block256,
                sdsDensityDensityOneSegment2048Block512,
                sdsDensityDensityOneSegment2048Block1024,
                sdsDensityDensityOneSegment2048Block2048,
                sdsDensityDensityOneSegment2048Block4096,

                sdsDensityDensityOneSegment4096Block16,
                sdsDensityDensityOneSegment4096Block32,
                sdsDensityDensityOneSegment4096Block64,
                sdsDensityDensityOneSegment4096Block128,
                sdsDensityDensityOneSegment4096Block256,
                sdsDensityDensityOneSegment4096Block512,
                sdsDensityDensityOneSegment4096Block1024,
                sdsDensityDensityOneSegment4096Block2048,
                sdsDensityDensityOneSegment4096Block4096,

                sdsDensityDensityTwoSegment256Block16,
                sdsDensityDensityTwoSegment256Block32,
                sdsDensityDensityTwoSegment256Block64,
                sdsDensityDensityTwoSegment256Block128,
                sdsDensityDensityTwoSegment256Block256,
                sdsDensityDensityTwoSegment256Block512,
                sdsDensityDensityTwoSegment256Block1024,
                sdsDensityDensityTwoSegment256Block2048,
                sdsDensityDensityTwoSegment256Block4096,

                sdsDensityDensityTwoSegment512Block16,
                sdsDensityDensityTwoSegment512Block32,
                sdsDensityDensityTwoSegment512Block64,
                sdsDensityDensityTwoSegment512Block128,
                sdsDensityDensityTwoSegment512Block256,
                sdsDensityDensityTwoSegment512Block512,
                sdsDensityDensityTwoSegment512Block1024,
                sdsDensityDensityTwoSegment512Block2048,
                sdsDensityDensityTwoSegment512Block4096,

                sdsDensityDensityTwoSegment1024Block16,
                sdsDensityDensityTwoSegment1024Block32,
                sdsDensityDensityTwoSegment1024Block64,
                sdsDensityDensityTwoSegment1024Block128,
                sdsDensityDensityTwoSegment1024Block256,
                sdsDensityDensityTwoSegment1024Block512,
                sdsDensityDensityTwoSegment1024Block1024,
                sdsDensityDensityTwoSegment1024Block2048,
                sdsDensityDensityTwoSegment1024Block4096,

                sdsDensityDensityTwoSegment2048Block16,
                sdsDensityDensityTwoSegment2048Block32,
                sdsDensityDensityTwoSegment2048Block64,
                sdsDensityDensityTwoSegment2048Block128,
                sdsDensityDensityTwoSegment2048Block256,
                sdsDensityDensityTwoSegment2048Block512,
                sdsDensityDensityTwoSegment2048Block1024,
                sdsDensityDensityTwoSegment2048Block2048,
                sdsDensityDensityTwoSegment2048Block4096,

                sdsDensityDensityTwoSegment4096Block16,
                sdsDensityDensityTwoSegment4096Block32,
                sdsDensityDensityTwoSegment4096Block64,
                sdsDensityDensityTwoSegment4096Block128,
                sdsDensityDensityTwoSegment4096Block256,
                sdsDensityDensityTwoSegment4096Block512,
                sdsDensityDensityTwoSegment4096Block1024,
                sdsDensityDensityTwoSegment4096Block2048,
                sdsDensityDensityTwoSegment4096Block4096,

                sdsDensityDensityThreeSegment256Block16,
                sdsDensityDensityThreeSegment256Block32,
                sdsDensityDensityThreeSegment256Block64,
                sdsDensityDensityThreeSegment256Block128,
                sdsDensityDensityThreeSegment256Block256,
                sdsDensityDensityThreeSegment256Block512,
                sdsDensityDensityThreeSegment256Block1024,
                sdsDensityDensityThreeSegment256Block2048,
                sdsDensityDensityThreeSegment256Block4096,

                sdsDensityDensityThreeSegment512Block16,
                sdsDensityDensityThreeSegment512Block32,
                sdsDensityDensityThreeSegment512Block64,
                sdsDensityDensityThreeSegment512Block128,
                sdsDensityDensityThreeSegment512Block256,
                sdsDensityDensityThreeSegment512Block512,
                sdsDensityDensityThreeSegment512Block1024,
                sdsDensityDensityThreeSegment512Block2048,
                sdsDensityDensityThreeSegment512Block4096,

                sdsDensityDensityThreeSegment1024Block16,
                sdsDensityDensityThreeSegment1024Block32,
                sdsDensityDensityThreeSegment1024Block64,
                sdsDensityDensityThreeSegment1024Block128,
                sdsDensityDensityThreeSegment1024Block256,
                sdsDensityDensityThreeSegment1024Block512,
                sdsDensityDensityThreeSegment1024Block1024,
                sdsDensityDensityThreeSegment1024Block2048,
                sdsDensityDensityThreeSegment1024Block4096,

                sdsDensityDensityThreeSegment2048Block16,
                sdsDensityDensityThreeSegment2048Block32,
                sdsDensityDensityThreeSegment2048Block64,
                sdsDensityDensityThreeSegment2048Block128,
                sdsDensityDensityThreeSegment2048Block256,
                sdsDensityDensityThreeSegment2048Block512,
                sdsDensityDensityThreeSegment2048Block1024,
                sdsDensityDensityThreeSegment2048Block2048,
                sdsDensityDensityThreeSegment2048Block4096,

                sdsDensityDensityThreeSegment4096Block16,
                sdsDensityDensityThreeSegment4096Block32,
                sdsDensityDensityThreeSegment4096Block64,
                sdsDensityDensityThreeSegment4096Block128,
                sdsDensityDensityThreeSegment4096Block256,
                sdsDensityDensityThreeSegment4096Block512,
                sdsDensityDensityThreeSegment4096Block1024,
                sdsDensityDensityThreeSegment4096Block2048,
                sdsDensityDensityThreeSegment4096Block4096);
        std::cout << std::endl << std::endl;
    }
    return 0;
}
