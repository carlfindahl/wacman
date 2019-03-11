# CMakeLists.txt for the GFX Pacman Assignment - Spring 2019
# 3.11 is needed for FetchContent
cmake_minimum_required(VERSION 3.11)

project(imt2531_assignment01 VERSION 1.0.2)

# CMake Complains if this is not set
set(OpenGL_GL_PREFERENCE GLVND)

add_subdirectory(pacman)
add_subdirectory(cglutil)
