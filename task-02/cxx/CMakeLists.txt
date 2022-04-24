cmake_minimum_required(VERSION 3.16)
project(ip_filter VERSION ${SEMVER})

if("${PROJECT_VERSION}" STREQUAL "")
    message(FATAL_ERROR)
endif()


add_executable(${PROJECT_NAME} main.cxx)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/)

set(CPACK_GENERATOR             "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT       "vladimir.u.ivanov@gmail.com")

include(CPack)
