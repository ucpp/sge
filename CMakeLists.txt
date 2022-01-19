cmake_minimum_required(VERSION 3.10)
project(ogl_render VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_DEMO "Build demo project" ON)

add_subdirectory(externals/glad)
add_subdirectory(externals/glfw)
add_subdirectory(externals/glm)
add_subdirectory(externals/imgui)
add_subdirectory(externals/assimp)
add_subdirectory(engine)

if(BUILD_DEMO)
	add_subdirectory(demo)
endif()
 
