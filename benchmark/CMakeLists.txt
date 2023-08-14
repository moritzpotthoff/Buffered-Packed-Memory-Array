project(Benchmark VERSION 1.0 LANGUAGES CXX)

# We need std::thread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Some warning flags, adjust to your needs
string(
  CONCAT WARNING_FLAGS
  "-Wall;"
  "-Wextra;"
  "-Wcast-align;"
  "-Wconversion;"
  "-Wnoexcept;"
  "-Wnon-virtual-dtor;"
  "-Wnull-dereference;"
  "-Woverloaded-virtual;"
  "-Wpedantic;"
  "-Wsuggest-attribute=const;"
  "-Wsuggest-attribute=noreturn;"
  "-Wshadow;"
  #"-Wsign-conversion;"
  "-Wsuggest-override;"
  "-Wundef;"
  "-Wunreachable-code;"
  "-Wunused;"
)

set(TBBLIBS TBB::tbb TBB::tbbmalloc)

add_executable(BatchBenchmark batchRandomDistributionBenchmark.cpp)
target_compile_features(BatchBenchmark PRIVATE cxx_std_20)
target_compile_options(BatchBenchmark PRIVATE $<$<CONFIG:Release>:-march=native>) # Native architecture compilation
target_compile_options(BatchBenchmark PRIVATE $<$<CONFIG:Release>:-ltbbmalloc_proxy>) # tbb scalable allocator
if(CMAKE_COMPILER_IS_GNUCXX)
  target_link_options(BatchBenchmark PRIVATE $<$<CONFIG:Release>:-flto>) # Use link-time optimization (not that we link that much)
endif()
target_compile_options(BatchBenchmark PRIVATE ${WARNING_FLAGS})
target_link_libraries(BatchBenchmark PRIVATE DataStructures)
target_link_libraries(BatchBenchmark PRIVATE Helpers::Helpers)
target_link_libraries(BatchBenchmark PRIVATE ${TBBLIBS})
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(BatchBenchmark PUBLIC OpenMP::OpenMP_CXX)
endif()

add_executable(BatchOverallocationFactorExperiment batchOverallocationFactorExperiment.cpp)
target_compile_features(BatchOverallocationFactorExperiment PRIVATE cxx_std_20)
target_compile_options(BatchOverallocationFactorExperiment PRIVATE $<$<CONFIG:Release>:-march=native>) # Native architecture compilation
target_compile_options(BatchOverallocationFactorExperiment PRIVATE $<$<CONFIG:Release>:-ltbbmalloc_proxy>) # tbb scalable allocator
if(CMAKE_COMPILER_IS_GNUCXX)
  target_link_options(BatchOverallocationFactorExperiment PRIVATE $<$<CONFIG:Release>:-flto>) # Use link-time optimization (not that we link that much)
endif()
target_compile_options(BatchOverallocationFactorExperiment PRIVATE ${WARNING_FLAGS})
target_link_libraries(BatchOverallocationFactorExperiment PRIVATE DataStructures)
target_link_libraries(BatchOverallocationFactorExperiment PRIVATE Helpers::Helpers)
target_link_libraries(BatchOverallocationFactorExperiment PRIVATE ${TBBLIBS})
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(BatchOverallocationFactorExperiment PUBLIC OpenMP::OpenMP_CXX)
endif()

add_executable(BatchBlockSizeExperiment batchBlockSizeExperiment.cpp)
target_compile_features(BatchBlockSizeExperiment PRIVATE cxx_std_20)
target_compile_options(BatchBlockSizeExperiment PRIVATE $<$<CONFIG:Release>:-march=native>) # Native architecture compilation
target_compile_options(BatchBlockSizeExperiment PRIVATE $<$<CONFIG:Release>:-ltbbmalloc_proxy>) # tbb scalable allocator
if(CMAKE_COMPILER_IS_GNUCXX)
  target_link_options(BatchBlockSizeExperiment PRIVATE $<$<CONFIG:Release>:-flto>) # Use link-time optimization (not that we link that much)
endif()
target_compile_options(BatchBlockSizeExperiment PRIVATE ${WARNING_FLAGS})
target_link_libraries(BatchBlockSizeExperiment PRIVATE DataStructures)
target_link_libraries(BatchBlockSizeExperiment PRIVATE Helpers::Helpers)
target_link_libraries(BatchBlockSizeExperiment PRIVATE ${TBBLIBS})
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(BatchBlockSizeExperiment PUBLIC OpenMP::OpenMP_CXX)
endif()

if(INCLUDE_COMPETITORS)
  add_executable(ParallelBSTBenchmark batchParallelBstBenchmark.cpp)
  target_compile_features(ParallelBSTBenchmark PRIVATE cxx_std_20)
  target_compile_options(ParallelBSTBenchmark PRIVATE $<$<CONFIG:Release>:-march=native>) # Native architecture compilation
  target_compile_options(ParallelBSTBenchmark PRIVATE $<$<CONFIG:Release>:-ltbbmalloc_proxy>) # tbb scalable allocator
  if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_options(ParallelBSTBenchmark PRIVATE $<$<CONFIG:Release>:-flto>) # Use link-time optimization (not that we link that much)
  endif()
  #target_compile_options(ParallelBSTBenchmark PRIVATE ${WARNING_FLAGS})
  target_link_libraries(ParallelBSTBenchmark PRIVATE DataStructures)
  target_link_libraries(ParallelBSTBenchmark PRIVATE Helpers::Helpers)
  target_link_libraries(ParallelBSTBenchmark PRIVATE ${TBBLIBS})
  target_link_libraries(ParallelBSTBenchmark PRIVATE ParallelBST)
  find_package(OpenMP)
  if(OpenMP_CXX_FOUND)
    target_link_libraries(ParallelBSTBenchmark PUBLIC OpenMP::OpenMP_CXX)
  endif()

  add_executable(CPMABenchmark cpmaBenchmark.cpp)
  target_compile_features(CPMABenchmark PRIVATE cxx_std_20)
  target_compile_options(CPMABenchmark PRIVATE $<$<CONFIG:Release>:-march=native>) # Native architecture compilation
  target_compile_options(CPMABenchmark PRIVATE $<$<CONFIG:Release>:-ltbbmalloc_proxy>) # tbb scalable allocator
  if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_options(CPMABenchmark PRIVATE $<$<CONFIG:Release>:-flto>) # Use link-time optimization (not that we link that much)
  endif()
  #target_compile_options(CPMABenchmark PRIVATE ${WARNING_FLAGS})
  target_link_libraries(CPMABenchmark PRIVATE DataStructures)
  target_link_libraries(CPMABenchmark PRIVATE Helpers::Helpers)
  target_link_libraries(CPMABenchmark PRIVATE ${TBBLIBS})
  target_link_libraries(CPMABenchmark PRIVATE CPMA::CPMA)
  find_package(OpenMP)
  if(OpenMP_CXX_FOUND)
    target_link_libraries(CPMABenchmark PUBLIC OpenMP::OpenMP_CXX)
  endif()

  add_executable(RangeQueryBenchmark batchRangeQueryBenchmark.cpp)
  target_compile_features(RangeQueryBenchmark PRIVATE cxx_std_20)
  target_compile_options(RangeQueryBenchmark PRIVATE $<$<CONFIG:Release>:-march=native>) # Native architecture compilation
  target_compile_options(RangeQueryBenchmark PRIVATE $<$<CONFIG:Release>:-ltbbmalloc_proxy>) # tbb scalable allocator
  if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_options(RangeQueryBenchmark PRIVATE $<$<CONFIG:Release>:-flto>) # Use link-time optimization (not that we link that much)
  endif()
  #target_compile_options(RangeQueryBenchmark PRIVATE ${WARNING_FLAGS})
  target_link_libraries(RangeQueryBenchmark PRIVATE DataStructures)
  target_link_libraries(RangeQueryBenchmark PRIVATE Helpers::Helpers)
  target_link_libraries(RangeQueryBenchmark PRIVATE ${TBBLIBS})
  target_link_libraries(RangeQueryBenchmark PRIVATE CPMA::CPMA)
  target_link_libraries(RangeQueryBenchmark PRIVATE ParallelBST)
  find_package(OpenMP)
  if(OpenMP_CXX_FOUND)
    target_link_libraries(RangeQueryBenchmark PUBLIC OpenMP::OpenMP_CXX)
  endif()
endif()

if(INCLUDE_TUNING)
  add_executable(BatchParameterTuning batchParameterTuningExperiment.cpp)
  target_compile_features(BatchParameterTuning PRIVATE cxx_std_20)
  target_compile_options(BatchParameterTuning PRIVATE $<$<CONFIG:Release>:-march=native>) # Native architecture compilation
  target_compile_options(BatchParameterTuning PRIVATE $<$<CONFIG:Release>:-ltbbmalloc_proxy>) # tbb scalable allocator
  if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_options(BatchParameterTuning PRIVATE $<$<CONFIG:Release>:-flto>) # Use link-time optimization (not that we link that much)
  endif()
  target_compile_options(BatchParameterTuning PRIVATE ${WARNING_FLAGS})
  target_link_libraries(BatchParameterTuning PRIVATE DataStructures)
  target_link_libraries(BatchParameterTuning PRIVATE Helpers::Helpers)
  target_link_libraries(BatchParameterTuning PRIVATE ${TBBLIBS})
  if(OpenMP_CXX_FOUND)
    target_link_libraries(BatchParameterTuning PUBLIC OpenMP::OpenMP_CXX)
  endif()
endif()

add_executable(CreateDistributionHistogram createDistributionHistogram.cpp)
target_compile_features(CreateDistributionHistogram PRIVATE cxx_std_20)
target_compile_options(CreateDistributionHistogram PRIVATE $<$<CONFIG:Release>:-march=native>) # Native architecture compilation
target_compile_options(CreateDistributionHistogram PRIVATE $<$<CONFIG:Release>:-ltbbmalloc_proxy>) # tbb scalable allocator
if(CMAKE_COMPILER_IS_GNUCXX)
  target_link_options(CreateDistributionHistogram PRIVATE $<$<CONFIG:Release>:-flto>) # Use link-time optimization (not that we link that much)
endif()
target_compile_options(CreateDistributionHistogram PRIVATE ${WARNING_FLAGS})
target_link_libraries(CreateDistributionHistogram PRIVATE DataStructures)
target_link_libraries(CreateDistributionHistogram PRIVATE Helpers::Helpers)
if(OpenMP_CXX_FOUND)
  target_link_libraries(CreateDistributionHistogram PUBLIC OpenMP::OpenMP_CXX)
endif()