project(helpers VERSION 1.0.0 LANGUAGES CXX)

add_library(Helpers INTERFACE)

target_compile_features(Helpers INTERFACE cxx_std_20)

target_include_directories(Helpers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library("Helpers::Helpers" ALIAS Helpers)
