project(core)

set(LIB_INC inc/data-hasher/data-hasher-iface.hpp
            inc/dup-detector/dup-detector-iface.hpp
            inc/item-reader/item-reader-iface.hpp
            inc/types.hpp)

set(LIB_SRC lib-stub.cpp)

add_library(${PROJECT_NAME} STATIC ${LIB_INC} ${LIB_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_compile_options(${PROJECT_NAME} PUBLIC
    -std=c++2a
    -fno-rtti
    -Wall
    -Wextra
    -Werror
    -pedantic)
