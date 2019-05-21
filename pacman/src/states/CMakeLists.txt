target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/state_manager.h
    ${CMAKE_CURRENT_LIST_DIR}/state_manager.cpp

    ${CMAKE_CURRENT_LIST_DIR}/state.h
    ${CMAKE_CURRENT_LIST_DIR}/state.cpp

    ${CMAKE_CURRENT_LIST_DIR}/editor_state.h
    ${CMAKE_CURRENT_LIST_DIR}/editor_state.cpp

    ${CMAKE_CURRENT_LIST_DIR}/game_state.h
    ${CMAKE_CURRENT_LIST_DIR}/game_state.cpp

    ${CMAKE_CURRENT_LIST_DIR}/game_over_state.h
    ${CMAKE_CURRENT_LIST_DIR}/game_over_state.cpp

    ${CMAKE_CURRENT_LIST_DIR}/respawn_state.h
    ${CMAKE_CURRENT_LIST_DIR}/respawn_state.cpp

    ${CMAKE_CURRENT_LIST_DIR}/pause_state.h
    ${CMAKE_CURRENT_LIST_DIR}/pause_state.cpp

    ${CMAKE_CURRENT_LIST_DIR}/help_state.h
    ${CMAKE_CURRENT_LIST_DIR}/help_state.cpp

    ${CMAKE_CURRENT_LIST_DIR}/main_menu_state.h
    ${CMAKE_CURRENT_LIST_DIR}/main_menu_state.cpp

    ${CMAKE_CURRENT_LIST_DIR}/high_score_state.h
    ${CMAKE_CURRENT_LIST_DIR}/high_score_state.cpp
)
