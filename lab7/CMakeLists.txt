cmake_minimum_required(VERSION 3.20)

project(lab7)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(BLAS REQUIRED)
find_package(benchmark REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(GTest REQUIRED)

add_executable(
    lab7
    "main.cpp"
    "AlignedAllocator.cpp"
    "Matrix.cpp"
    "MatrixInv.cpp"
    "MatrixMult.cpp"
    "AlignedAllocator.hpp"
    "Matrix.hpp"
    "MatrixInv.hpp"
    "MatrixMult.hpp")
set_property(
    TARGET lab7
    PROPERTY COMPILE_FLAGS
             "-Wall -O3 -ftree-vectorize -march=native -mtune=native")
target_link_libraries(lab7 BLAS::BLAS benchmark::benchmark Eigen3::Eigen gtest
                      gtest_main)

enable_testing()

add_executable(InversionTest)
target_sources(
    InversionTest
    PRIVATE "inversion_test.cpp"
            "AlignedAllocator.cpp"
            "Matrix.cpp"
            "MatrixInv.cpp"
            "MatrixMult.cpp"
            "AlignedAllocator.hpp"
            "Matrix.hpp"
            "MatrixInv.hpp"
            "MatrixMult.hpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})
target_link_libraries(InversionTest BLAS::BLAS Eigen3::Eigen gtest gtest_main)
set_property(
    TARGET InversionTest
    PROPERTY COMPILE_FLAGS
             "-Wall -O3 -ftree-vectorize -march=native -mtune=native")
add_test(NAME InversionTest COMMAND InversionTest)
