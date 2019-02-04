# Pacman Subdirectory

# Set name and add executable (specify main.cpp here so list of sources is not empty)
set(EXEC_NAME pacman)
add_executable(${EXEC_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

# Add this so it can specify source files and include directories local to it's own directory
include(${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt)

# CMake Complains if this is not set
set(OpenGL_GL_PREFERENCE GLVND)

# Discover and link required libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 3.2 REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(
    ${EXEC_NAME}
    PRIVATE
    OpenGL::GL                      # For OpenGL
    glfw                            # For window integration
    glm                             # For Maths

    # Manually link asan on GCC since it doesn't do it automatically
    $<IF:$<CXX_COMPILER_ID:GNU>,asan,>
)

# Enable address sanitizer for debug builds that run on GCC or Clang
target_compile_options(
    ${EXEC_NAME}
    PRIVATE

    # On Clang or GCC, Enable address sanitizer to detect nasty bugs
    $<IF:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,-fsanitize=address,>
)

# Explicitly tell what features are required to maximize compiler support
target_compile_features(
    ${EXEC_NAME}
    PRIVATE
    cxx_std_17
)