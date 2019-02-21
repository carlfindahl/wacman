# Pacman Subdirectory

# Set name and add executable (specify main.cpp here so list of sources is not empty)
set(EXEC_NAME pacman)
add_executable(${EXEC_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

# Add this so it can specify source files and include directories local to it's own directory
include(${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt)

# Discover and link required libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 3.2 REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(
    ${EXEC_NAME}
    PRIVATE
    # Manually link asan on GCC in Debug mode since it doesn't do it automatically (MUST BE FIRST IN THE LIST OR WON'T WORK!)
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Debug>>:asan>

    $<$<PLATFORM_ID:Linux>:dl>      # Required by glad on Linux
    OpenGL::GL                      # For OpenGL
    glfw                            # For window integration
    glm                             # For Maths
    cgl                             # OpenGL utilities (custom library build in-source in this project)
)

# Enable address sanitizer for debug builds that run on GCC or Clang
target_compile_options(
    ${EXEC_NAME}
    PRIVATE

    # On GCC in Debug mode Enable address sanitizer to detect nasty bugs
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Debug>>:-fsanitize=address>
)

# Explicitly tell what features are required to maximize compiler support
target_compile_features(
    ${EXEC_NAME}
    PRIVATE
    cxx_std_17
)

# Ensure resource files are available by copying
add_custom_command(
    TARGET ${EXEC_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
    COMMAND ${CMAKE_COMMAND} -E echo "Copied resource directory to binary directory."
)
