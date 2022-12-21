project(cli)

set(EXE ${CMAKE_PROJECT_NAME}-${PROJECT_NAME})
add_executable(${EXE} main.cpp)

install(TARGETS ${EXE} RUNTIME DESTINATION bin/)
