cmake_minimum_required(VERSION 3.13)  # CMake version check
project(swann)                        # Create project "swann"
set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard

# Add main.cpp file of project root directory as source file
file( GLOB SOURCE_FILES
  "util/*.hpp"
  "hash/*.hpp"
  "index/*.hpp"
  "dataset/*.hpp"
  "*.hpp"
)

# Add output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /swann/bin)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(swann ${SOURCE_FILES} "main.cpp")

# Testing
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

include(GoogleTest)
 
add_executable(
  hello_test
  ${SOURCE_FILES}
  "test/test.cc"
  "test/index/bfindex.cc"
  "test/index/lshtrie.cc"
  "test/hash/hashfamily.cc"
)
target_link_libraries(
  hello_test
  gtest
  gtest_main
)

gtest_discover_tests(hello_test)
