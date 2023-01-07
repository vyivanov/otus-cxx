cmake_minimum_required(VERSION 3.18)
project(cppinference LANGUAGES CXX)

find_package(CUDAToolkit REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

set(LIB_INC_PUB inc/pub/classifier/classifier.hpp
                inc/pub/classifier/logistic-regression.hpp
                inc/pub/tools.inl
                inc/pub/tools.hpp
                inc/pub/types.hpp)

set(LIB_INC_PRIV inc/priv/logistic-regression.hpp
                 inc/priv/types.hpp)

set(LIB_SRC src/logistic-regression.cpp
            src/impl/logistic-regression.cpp)

add_library(${PROJECT_NAME} SHARED ${LIB_INC_PRIV} ${LIB_INC_PUB} ${LIB_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE inc/)

add_executable(${PROJECT_NAME}-main main.cpp)
target_include_directories(${PROJECT_NAME}-main PRIVATE inc/)
target_link_libraries(${PROJECT_NAME}-main PRIVATE ${PROJECT_NAME} CUDA::cudart_static)
