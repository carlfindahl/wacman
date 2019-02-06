# Add source files local to this directory
target_sources(
    ${EXEC_NAME}
    PRIVATE

    ${CMAKE_CURRENT_LIST_DIR}/input.h
    ${CMAKE_CURRENT_LIST_DIR}/input.cpp

    ${CMAKE_CURRENT_LIST_DIR}/game.h
    ${CMAKE_CURRENT_LIST_DIR}/game.cpp

    ${CMAKE_CURRENT_LIST_DIR}/gfx.h
    ${CMAKE_CURRENT_LIST_DIR}/single_header_implementations.cpp
)

target_include_directories(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Add any subdirectories with source files
include(${CMAKE_CURRENT_LIST_DIR}/glad/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/imgui/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/states/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/rendering/CMakeLists.txt)
