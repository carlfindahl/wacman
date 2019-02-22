# Add source files local to this directory
target_sources(
    ${EXEC_NAME}
    PRIVATE

    ${CMAKE_CURRENT_LIST_DIR}/input.h
    ${CMAKE_CURRENT_LIST_DIR}/input.cpp

    ${CMAKE_CURRENT_LIST_DIR}/game.h
    ${CMAKE_CURRENT_LIST_DIR}/game.cpp

    ${CMAKE_CURRENT_LIST_DIR}/pacman.h
    ${CMAKE_CURRENT_LIST_DIR}/pacman.cpp

    ${CMAKE_CURRENT_LIST_DIR}/ghost.h
    ${CMAKE_CURRENT_LIST_DIR}/ghost.cpp

    ${CMAKE_CURRENT_LIST_DIR}/level.h
    ${CMAKE_CURRENT_LIST_DIR}/level.cpp

    ${CMAKE_CURRENT_LIST_DIR}/pathfinding.h
    ${CMAKE_CURRENT_LIST_DIR}/pathfinding.cpp

    ${CMAKE_CURRENT_LIST_DIR}/common.h
    ${CMAKE_CURRENT_LIST_DIR}/common.cpp

    ${CMAKE_CURRENT_LIST_DIR}/gfx.h
    ${CMAKE_CURRENT_LIST_DIR}/single_header_implementations.cpp
)

target_include_directories(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_BINARY_DIR} # for the configured file (config.h)
)

# Compile Options
option(PACMAN_BUILD_BENCHMARKS OFF)

target_compile_definitions(
    ${EXEC_NAME}
    PRIVATE
    $<$<BOOL:${PACMAN_BUILD_BENCHMARKS}>:PACMAN_USE_BENCHMARKS>
)

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/config.h.in
    config.h
)

# Add any subdirectories with source files
include(${CMAKE_CURRENT_LIST_DIR}/glad/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/imgui/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/states/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/rendering/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/benchmarks/CMakeLists.txt)
