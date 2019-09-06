# AI source files

target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/behaviour_tree.h
    ${CMAKE_CURRENT_LIST_DIR}/behaviour_tree.cpp
    
    ${CMAKE_CURRENT_LIST_DIR}/nodes.h
    ${CMAKE_CURRENT_LIST_DIR}/nodes.cpp
)
