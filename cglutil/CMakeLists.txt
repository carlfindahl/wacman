# Library with OpenGL utilities to make life easier for this course

set(CGL_UTIL_NAME "cgl")
add_library(${CGL_UTIL_NAME} ${CMAKE_BUILD_SHARED_LIBS} "")

find_package(OpenGL REQUIRED)
find_package(glfw3 3.2)

# Define CGL_USE_GLFW if we have access to GLFW
if(${glfw3_FOUND})
    message(status "Adding GLFW support to CGL.")
    target_compile_definitions(${CGL_UTIL_NAME}
                               PUBLIC
                               CGL_USE_GLFW
        )
endif()

# Add source files
target_sources(
    ${CGL_UTIL_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include/cglutil.h

    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/cglutil.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/stb_image.h
    ${CMAKE_CURRENT_LIST_DIR}/src/implementations.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/glad/glad.h
    ${CMAKE_CURRENT_LIST_DIR}/src/glad.c
)

# Set up dependencies
target_link_libraries(
    ${CGL_UTIL_NAME}
    PRIVATE
    OpenGL::GL
    glfw
    $<IF:$<CXX_COMPILER_ID:GNU>,stdc++fs,> # GCC requires manual fs link
)

target_include_directories(
    ${CGL_UTIL_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include

    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

# CGL Uses C++ 17 internally, but the public interface should be C++11 compatible
target_compile_features(
    ${CGL_UTIL_NAME}
    PUBLIC
    cxx_std_11

    PRIVATE
    cxx_std_17
)
