# Add ImGui in source as this is the recommended way to use it
target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/imconfig.h
    ${CMAKE_CURRENT_LIST_DIR}/imgui.h
    ${CMAKE_CURRENT_LIST_DIR}/imgui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_demo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_widgets.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_impl_glfw.h
    ${CMAKE_CURRENT_LIST_DIR}/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_impl_opengl3.h
    ${CMAKE_CURRENT_LIST_DIR}/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui_internal.h
    ${CMAKE_CURRENT_LIST_DIR}/imstb_rectpack.h
    ${CMAKE_CURRENT_LIST_DIR}/imstb_textedit.h
    ${CMAKE_CURRENT_LIST_DIR}/imstb_truetype.h

)