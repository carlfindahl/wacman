# Add GFX from the Course as a Library that we can link with

add_library(gfx STATIC)
add_library(gfx::gfx ALIAS gfx)

# Add source files
target_sources(
    gfx

    # Public Sources
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include/gfx.h

    # Private Sources
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/gfx.cpp
)

# Set include directories
target_include_directories(
    gfx
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# GFX Depends privately on glad to build
target_link_libraries(gfx PRIVATE glad)
