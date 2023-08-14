project(parallel_bst VERSION 1.0.0 LANGUAGES CXX)

add_library(ParallelBST INTERFACE)

target_compile_features(ParallelBST INTERFACE cxx_std_20)

target_include_directories(ParallelBST INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# The namespace alias can be used as link target if this project is a
# subproject.
add_library("ParallelBST::ParallelBST" ALIAS ParallelBST)
