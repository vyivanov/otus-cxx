cmake_minimum_required(VERSION 3.18)
project(cppinference LANGUAGES CXX)

find_package(Eigen3 REQUIRED)
set(CMAKE_CXX_STANDARD 20)

set(LIB_INC_PUB inc/pub/tools.inl
                inc/pub/tools.hpp
                inc/pub/types.hpp)

set(LIB_SRC src/logistic-regression.cpp)

add_library(${PROJECT_NAME} SHARED ${LIB_INC_PUB} ${LIB_SRC})

add_executable(${PROJECT_NAME}-main main.cpp)
