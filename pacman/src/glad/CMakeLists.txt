# Add glad source files and proper include dir

target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/glad.c
    ${CMAKE_CURRENT_LIST_DIR}/glad.h
)
