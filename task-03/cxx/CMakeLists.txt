cmake_minimum_required(VERSION 3.16)
project(contaloc VERSION ${SEMVER})

if("${PROJECT_VERSION}" STREQUAL "")
    message(FATAL_ERROR)
endif()

find_library(LIB_FMT fmt REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)

option(LOGGING_ON "turn on logging?" NO)
if(${LOGGING_ON})
    target_compile_definitions(${PROJECT_NAME} PRIVATE "LOGGING_ON")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror)

# TODO: implement container component
add_subdirectory(allocator)
add_subdirectory(tool)

target_link_libraries(${PROJECT_NAME} PRIVATE allocator tool ${LIB_FMT})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/)

set(CPACK_GENERATOR                "DEB")
set(CPACK_DEBIAN_FILE_NAME         "DEB-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS "ON")
set(CPACK_PACKAGE_VERSION_MAJOR    "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR    "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH    "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT          "vladimir.u.ivanov@gmail.com")

include(CPack)
