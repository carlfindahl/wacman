# Entity source files 

target_sources(
    ${EXEC_NAME}
    PRIVATE
 
    "${CMAKE_CURRENT_LIST_DIR}/system.h"
 
    "${CMAKE_CURRENT_LIST_DIR}/components.h"

    "${CMAKE_CURRENT_LIST_DIR}/movement_system.h"
    "${CMAKE_CURRENT_LIST_DIR}/movement_system.cpp"
)