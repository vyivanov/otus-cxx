project(core)

set(LIB_INC)

set(LIB_SRC lib-stub.cpp)

add_library(${PROJECT_NAME} STATIC ${LIB_INC} ${LIB_SRC})
