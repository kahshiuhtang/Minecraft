cmake_minimum_required(VERSION 3.11)
project(minecraft VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "-lstdc++fs -std=c++17")
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

include_directories(${BULLET_INCLUDE_DIR})
find_package(Bullet CONFIG REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(BULLET_PHYSICS_SOURCE_DIR ${CMAKE_SOURCE_DIR})

include_directories(include)

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/0.9.9.8
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
	assimp
	GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.2

)

FetchContent_MakeAvailable(assimp)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

add_executable(minecraft 
                src/main.cpp
                src/glad.c 
                src/camera.cpp
                src/renderer.cpp
                src/shader.cpp
                src/screen.cpp
                src/player.cpp
                src/world.cpp
                include/camera.hpp
                include/renderer.hpp
                include/screen.hpp
                include/shader.hpp
                include/player.hpp
                include/world.hpp
                )

target_include_directories(minecraft PRIVATE include)
target_compile_definitions(minecraft PUBLIC ${BULLET_DEFINITIONS})
target_link_directories(minecraft PUBLIC ${Bullet_DIR}/${BULLET_ROOT_DIR}/${BULLET_LIBRARY_DIRS})
target_link_libraries(minecraft 
                        glfw
                        OpenGL::GL
                        glm::glm
                        assimp::assimp
                        )
target_link_libraries(minecraft BulletDynamics BulletCollision LinearMath)