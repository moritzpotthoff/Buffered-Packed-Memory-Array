set(TBBLIBS TBB::tbb TBB::tbbmalloc)

add_executable(batchTests batchedSparseArrayTest.cpp)
target_link_libraries(batchTests PRIVATE Catch2::Catch2WithMain)
target_compile_features(batchTests PRIVATE cxx_std_20)
target_compile_options(batchTests PRIVATE $<$<CONFIG:Release>:-march=native>)
if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_options(batchTests PRIVATE $<$<CONFIG:Release>:-flto>)
endif()
target_link_libraries(batchTests PRIVATE DataStructures)
target_link_libraries(batchTests PRIVATE Helpers::Helpers)
target_link_libraries(batchTests PRIVATE ${TBBLIBS})

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(batchTests PUBLIC OpenMP::OpenMP_CXX)
endif()


