cmake_minimum_required(VERSION 3.20)  # CMake version check
project(swann)                        # Create project "swann"
set(CMAKE_CXX_STANDARD 23)            # Enable c++23 standard

# Add main.cpp file of project root directory as source file
file( GLOB SOURCE_FILES
  "util/*.hpp"
  "hash/*.hpp"
  "index/*.hpp"
  "dataset/*.hpp"
  "*.hpp"
)

# Link to HDF5
find_package(HDF5 REQUIRED COMPONENTS C CXX HL)

# Add output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /swann/bin)


include_directories(
  ON
  /usr/include/hdf5/serial
)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(swann ${SOURCE_FILES} "main.cpp")

target_link_libraries(
  swann
  ${HDF5}
  ${HDF5_LIBRARIES}
  ${HDF5_CXX_LIBRARIES}
  ${HDF5_HL_LIBRARIES}
  ${HDF5_CXX_HL_LIBRARIES}
  /usr/include/hdf5/serial/hdf5.h
)


# Testing
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

include(GoogleTest)
 
file(GLOB TEST_UTIL_FILES
  "test/index/util.hpp"
)

add_executable(
  hello_test
  ${SOURCE_FILES}
  ${TEST_UTIL_FILES}
  "test/test.cc"
  "test/util/ranges.cc"
  "test/hash/hashfamily.cc"
  "test/hash/hashpool.cc"
  "test/index/point.cc"
  "test/hash/hashfamilyfactory.cc"
  "test/index/lsharraymap.cc"
  "test/index/lshmapfactory.cc"
  "test/index/pointarithmeticresult.cc"
  "test/index/lshforest.cc"
  "test/index/bfindex.cc"
  "test/index/lshtrie.cc"
)

target_link_libraries(
  hello_test
  gtest
  gtest_main
  ${HDF5_LIBRARIES}
)

gtest_discover_tests(hello_test)


# Benchmarking
file(GLOB BENCHMARK_FILES
  "benchmark/*.cc"
)

find_package(benchmark REQUIRED)

add_executable(
  benchmark
  ${SOURCE_FILES}
  ${BENCHMARK_FILES}
)

target_link_libraries(
  benchmark
  benchmark::benchmark
  ${HDF5_LIBRARIES}
)