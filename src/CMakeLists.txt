cmake_minimum_required(VERSION 3.13)  # CMake version check
project(swann)                        # Create project "swann"
set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# Testing
# enable_testing()
# include(GoogleTest)
 
# add_executable(
#   hello_test
#   test.cc
# )
# target_link_libraries(
#   hello_test
#   GTest::gtest_main
# )

# gtest_discover_tests(hello_test)



# Add main.cpp file of project root directory as source file
set(SOURCE_FILES main.cpp)

# Add output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /swann/bin)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(swann ${SOURCE_FILES})
