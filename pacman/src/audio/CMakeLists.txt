# Add source files for audio subsystem

if (${OpenAL_FOUND})
    target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/waveloader.h

        ${CMAKE_CURRENT_LIST_DIR}/sound_manager.h
        ${CMAKE_CURRENT_LIST_DIR}/sound_manager.cpp
    )
endif()
