cmake_minimum_required(VERSION 3.16)
project(tensor VERSION ${SEMVER})

if("${PROJECT_VERSION}" STREQUAL "")
    message(FATAL_ERROR)
endif()

add_executable(${PROJECT_NAME} main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror)

add_subdirectory(tensor-impl)
target_link_libraries(${PROJECT_NAME} PRIVATE tensor-impl)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/)

set(CPACK_GENERATOR                "DEB")
set(CPACK_DEBIAN_FILE_NAME         "DEB-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS "ON")
set(CPACK_PACKAGE_VERSION_MAJOR    "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR    "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH    "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT          "vladimir.u.ivanov@gmail.com")

include(CPack)
