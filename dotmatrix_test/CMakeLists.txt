cmake_minimum_required(VERSION 2.6.4)
project(gbtest)
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(gbtest cpu_test.cpp ../dotmatrix_src/cpu.cpp)

target_link_libraries(gbtest gtest gtest_main)