cmake_minimum_required(VERSION 3.16)
project(contaloc VERSION ${SEMVER})

if("${PROJECT_VERSION}" STREQUAL "")
    message(FATAL_ERROR)
endif()

option(LOGGING_ON      "turn on logging?"      NO)
option(OPTIMIZE_ALLOCS "optimize allocations?" NO)

add_executable(${PROJECT_NAME} main.cpp)

if(${LOGGING_ON})
    target_compile_definitions(${PROJECT_NAME} PRIVATE "LOGGING_ON")
endif()
if(${OPTIMIZE_ALLOCS})
    target_compile_definitions(${PROJECT_NAME} PRIVATE "OPTIMIZE_ALLOCS")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror)

add_subdirectory(allocator)
target_link_libraries(${PROJECT_NAME} PRIVATE allocator)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/)

set(CPACK_GENERATOR                "DEB")
set(CPACK_DEBIAN_FILE_NAME         "DEB-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS "ON")
set(CPACK_PACKAGE_VERSION_MAJOR    "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR    "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH    "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT          "vladimir.u.ivanov@gmail.com")

include(CPack)
