cmake_minimum_required(VERSION 3.20)

project(PCLabs)

set(CMAKE_CXX_STANDARD 20)

find_package(benchmark REQUIRED)

add_executable(PCLabs main.cpp)
target_link_libraries(PCLabs benchmark::benchmark)
