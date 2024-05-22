cmake_minimum_required(VERSION 3.0.0)
project(opengl_planet_simulator VERSION 0.1.0)

cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

include_directories(include)

add_executable(opengl_planet_simulator 
                src/main.cpp
                src/glad.c 
                )

target_link_libraries(opengl_planet_simulator 
                        glfw
                        OpenGL::GL)