get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

add_library(${COMPONENT_NAME} INTERFACE)

target_include_directories(
    ${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
