project(cli)

set(Boost_USE_STATIC_LIBS YES)
find_package(Boost REQUIRED COMPONENTS program_options)

set(EXE ${CMAKE_PROJECT_NAME}-${PROJECT_NAME})
add_executable(${EXE} main.cpp)

target_include_directories(${EXE} PRIVATE Boost::program_options)
target_link_libraries(${EXE} PRIVATE core Boost::program_options)

install(TARGETS ${EXE} RUNTIME DESTINATION bin/)
