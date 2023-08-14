cmake_minimum_required(VERSION 3.16)

# Forbid in-tree builds
get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)
if("${srcdir}" STREQUAL "${bindir}")
  message(NOTICE "")
  message(NOTICE "In-tree builds are not supported.")
  message(NOTICE "It is recommended to use a separate build directory")
  message(NOTICE "    cmake -B <build_dir>")
  message(FATAL_ERROR "Aborting generation...")
endif()

project(SparseArrayProject VERSION 1.0 LANGUAGES CXX)

#make the preset name accessible in the code
configure_file(
        "cmake_config.hpp.in"
        "cmake_config.hpp"
)
#and allow the code to use the file
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#load TBB
find_package(TBB 2021.9.0 EXACT REQUIRED PATHS ./lib/oneTBBinstalled/)

if(NOT TBB_FOUND)
  message(SEND_ERROR "TBB not found")
endif()


if(INCLUDE_COMPETITORS)
  add_subdirectory(parallelBST)
  add_subdirectory(CPMA)
endif()
add_subdirectory(dataStructures)
add_subdirectory(benchmark)
add_subdirectory(helpers)

#include tests if necessary
if(INCLUDE_TESTS)
  message(WARNING "Building with Catch2 tests. Be aware that this is very slow.")
  add_subdirectory(lib/Catch2)
  add_subdirectory(tests)
endif()