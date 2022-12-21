cmake_minimum_required(VERSION 3.16)
project(dup-detector VERSION ${SEMVER})

if("${PROJECT_VERSION}" STREQUAL "")
    message(FATAL_ERROR)
endif()

set(CPACK_GENERATOR                "DEB")
set(CPACK_DEBIAN_FILE_NAME         "DEB-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS "ON")
set(CPACK_DEBIAN_PACKAGE_SECTION   "utils")
set(CPACK_PACKAGE_DESCRIPTION      "Tool to find object duplicates by it's content")
set(CPACK_PACKAGE_CONTACT          "vladimir.u.ivanov@gmail.com")
set(CPACK_PACKAGE_VERSION_MAJOR    "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR    "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH    "${PROJECT_VERSION_PATCH}")

include(CPack)
