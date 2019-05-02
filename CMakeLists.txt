cmake_minimum_required(VERSION 2.6)
project(flyweight_circular_buffer)

################################
# GTest
################################
ADD_SUBDIRECTORY (gtest-1.8.1)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-std=c++17)

################################
# Unit Tests
################################
# Add test cpp file
add_executable(unit_tests unit_tests.cc)
add_executable(example example.cc)

# Link test executable against gtest & gtest_main
target_link_libraries(unit_tests gtest gtest_main)
add_test(unit_tests unit_tests)

