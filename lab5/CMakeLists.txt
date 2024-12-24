cmake_minimum_required(VERSION 3.20)

project(lab5)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(lab5 main.cpp)
target_link_libraries(lab5 ${OpenCV_LIBS})
