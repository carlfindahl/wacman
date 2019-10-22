# CMakeLists.txt for the GFX Pacman Assignment - Spring 2019
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(imt2531_assignment01 VERSION 1.1.1)

# CMake Complains if this is not set
set(OpenGL_GL_PREFERENCE GLVND)

add_subdirectory(cglutil)
add_subdirectory(external)
add_subdirectory(pacman)
