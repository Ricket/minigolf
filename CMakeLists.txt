cmake_minimum_required(VERSION 2.0)
PROJECT(Minigolf)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(GLUT)

SET(CMAKE_C_FLAGS "-g -Wall")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)
