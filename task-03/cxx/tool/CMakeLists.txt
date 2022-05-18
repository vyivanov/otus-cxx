get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
find_library(LIB_FMT fmt REQUIRED)

add_library(${COMPONENT_NAME} INTERFACE)

target_include_directories(
    ${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(
    ${COMPONENT_NAME} INTERFACE ${LIB_FMT})
