# CMakeLists.txt for the GFX Pacman Assignment - Spring 2019
cmake_minimum_required(VERSION 3.10)

project(imt2531_assignment01 VERSION 0.9.0)

# CMake Complains if this is not set
set(OpenGL_GL_PREFERENCE GLVND)

add_subdirectory(pacman)
add_subdirectory(cglutil)
