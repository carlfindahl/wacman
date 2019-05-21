# Entity source files 

target_sources(
    ${EXEC_NAME}
    PRIVATE
 
    "${CMAKE_CURRENT_LIST_DIR}/events.h"

    "${CMAKE_CURRENT_LIST_DIR}/system.h"
 
    "${CMAKE_CURRENT_LIST_DIR}/components.h"

    "${CMAKE_CURRENT_LIST_DIR}/factory.h"
    "${CMAKE_CURRENT_LIST_DIR}/factory.cpp"

    "${CMAKE_CURRENT_LIST_DIR}/input_system.h"
    "${CMAKE_CURRENT_LIST_DIR}/input_system.cpp"

    "${CMAKE_CURRENT_LIST_DIR}/game_system.h"
    "${CMAKE_CURRENT_LIST_DIR}/game_system.cpp"

    "${CMAKE_CURRENT_LIST_DIR}/ai_system.h"
    "${CMAKE_CURRENT_LIST_DIR}/ai_system.cpp"

    "${CMAKE_CURRENT_LIST_DIR}/movement_system.h"
    "${CMAKE_CURRENT_LIST_DIR}/movement_system.cpp"

    "${CMAKE_CURRENT_LIST_DIR}/animation_system.h"
    "${CMAKE_CURRENT_LIST_DIR}/animation_system.cpp"

    "${CMAKE_CURRENT_LIST_DIR}/rendering_system.h"
    "${CMAKE_CURRENT_LIST_DIR}/rendering_system.cpp"
)