cmake_minimum_required(VERSION 3.0.0)
project(strum VERSION 0.1.0)
set(OpenGL_GL_PREFERENCE "GLVND")

add_subdirectory(raylib)
add_executable(strum main.cpp)
target_link_libraries(strum raylib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
