project(cpma VERSION 1.0.0 LANGUAGES CXX)

add_library(CPMA INTERFACE)

target_compile_features(CPMA INTERFACE cxx_std_20)

target_compile_definitions(CPMA INTERFACE PARLAY=1)

find_package(Threads REQUIRED)
find_package(Parlay CONFIG REQUIRED PATHS ../lib/parlaylib-install/)

target_include_directories(CPMA INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(CPMA INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CPMA-test-repo)
target_include_directories(CPMA INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CPMA-test-repo/tlx)
target_include_directories(CPMA INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CPMA-test-repo/ParallelTools)
target_include_directories(CPMA INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CPMA-test-repo/parlaylib)

target_link_libraries(CPMA INTERFACE Parlay::parlay)

add_library("CPMA::CPMA" ALIAS CPMA)
