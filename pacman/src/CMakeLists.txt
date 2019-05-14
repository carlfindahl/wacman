# Add source files local to this directory
target_sources(
    ${EXEC_NAME}
    PRIVATE

    ${CMAKE_CURRENT_LIST_DIR}/game.h
    ${CMAKE_CURRENT_LIST_DIR}/game.cpp

    ${CMAKE_CURRENT_LIST_DIR}/level.h
    ${CMAKE_CURRENT_LIST_DIR}/level.cpp

    ${CMAKE_CURRENT_LIST_DIR}/pathfinding.h
    ${CMAKE_CURRENT_LIST_DIR}/pathfinding.cpp

    ${CMAKE_CURRENT_LIST_DIR}/reflect.h
    ${CMAKE_CURRENT_LIST_DIR}/reflect.cpp

    ${CMAKE_CURRENT_LIST_DIR}/common.h
    ${CMAKE_CURRENT_LIST_DIR}/common.cpp

    ${CMAKE_CURRENT_LIST_DIR}/single_header_implementations.cpp
)

target_include_directories(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_BINARY_DIR} # for the configured file (config.h)
)

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/config.h.in
    config.h
)

# Add any subdirectories with source files
include(${CMAKE_CURRENT_LIST_DIR}/audio/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/encrypt/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/entity/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/imgui/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/input/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/rendering/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/robinhood/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/states/CMakeLists.txt)
