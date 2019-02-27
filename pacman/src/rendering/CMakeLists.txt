target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/stb_image.h
    
    ${CMAKE_CURRENT_LIST_DIR}/renderer.h
    ${CMAKE_CURRENT_LIST_DIR}/renderer.cpp

    ${CMAKE_CURRENT_LIST_DIR}/shader_program.h
    ${CMAKE_CURRENT_LIST_DIR}/shader_program.cpp

    ${CMAKE_CURRENT_LIST_DIR}/uniform_buffer_object.h

    ${CMAKE_CURRENT_LIST_DIR}/vertex_array_object.h
    ${CMAKE_CURRENT_LIST_DIR}/vertex_array_object.cpp

    ${CMAKE_CURRENT_LIST_DIR}/post_processing.h
    ${CMAKE_CURRENT_LIST_DIR}/post_processing.cpp
)
