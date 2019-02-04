# Add source files local to this directory
target_sources(
    ${EXEC_NAME}
    PRIVATE
)

target_include_directories(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}    
)