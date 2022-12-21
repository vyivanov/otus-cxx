project(test)
find_package(GTest REQUIRED)

set(TEST_NAME
    ${PROJECT_NAME}-dup-detector-impl
    ${PROJECT_NAME}-stub-item-reader)

set(TEST_FILE
    dup-detector/dup-detector-impl.cpp
    item-reader/stub-item-reader.cpp)

foreach(name file IN ZIP_LISTS TEST_NAME TEST_FILE)
    add_executable(${name} main.cpp ${file})
    target_link_libraries(${name} PRIVATE core GTest::GTest)
    add_test(NAME ${name} COMMAND ${name})
endforeach()
