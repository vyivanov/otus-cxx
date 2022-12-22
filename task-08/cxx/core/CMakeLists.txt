project(core)

set(LIB_INC)

set(LIB_SRC lib-stub.cpp)

add_library(${PROJECT_NAME} STATIC ${LIB_INC} ${LIB_SRC})

target_compile_options(${PROJECT_NAME} PUBLIC
    -std=c++2a
    -fno-rtti
    -Wall
    -Wextra
    -Werror
    -pedantic)
