cmake_minimum_required(VERSION 3.18)
project(cxx)

find_package(Eigen3 REQUIRED)
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)
