project(test)
find_package(GTest REQUIRED)

set(TEST_NAME test-stub)

set(TEST_FILE test-stub.cpp)

foreach(name file IN ZIP_LISTS TEST_NAME TEST_FILE)
    add_executable(${name} main.cpp ${file})
    target_link_libraries(${name} PRIVATE GTest::GTest)
    add_test(NAME ${name} COMMAND ${name})
endforeach()
