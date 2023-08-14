project(data_structures VERSION 1.0.0 LANGUAGES CXX)

add_library(DataStructures INTERFACE)

target_compile_features(DataStructures INTERFACE cxx_std_20)

target_include_directories(DataStructures INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library("DataStructures::DataStructures" ALIAS DataStructures)
