project(core)

set(LIB_INC inc/data-hasher/boost-data-hasher.hpp
            inc/data-hasher/data-hasher-iface.hpp
            inc/dup-detector/dup-detector-iface.hpp
            inc/dup-detector/dup-detector-impl.hpp
            inc/item-reader/item-reader-iface.hpp
            inc/item-reader/local-file-reader.hpp
            inc/item-reader/stub-item-reader.hpp
            inc/types.hpp)

set(LIB_SRC src/data-hasher/boost-data-hasher.cpp
            src/dup-detector/dup-detector-impl.cpp
            src/item-reader/local-file-reader.cpp
            src/item-reader/stub-item-reader.cpp)

add_library(${PROJECT_NAME} STATIC ${LIB_INC} ${LIB_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_compile_options(${PROJECT_NAME} PUBLIC
    -std=c++2a
    -fno-rtti
    -Wall
    -Wextra
    -Werror
    -pedantic)
