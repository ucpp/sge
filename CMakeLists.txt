cmake_minimum_required(VERSION 3.10)
project(ogl_render VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_DEMO "Build demo project" ON)

add_subdirectory(externals/glad)
add_subdirectory(externals/glfw)
add_subdirectory(engine/sources)

if(BUILD_DEMO)
    file(GLOB ENGINE_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/engine/shaders/*.*)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/demo/shaders/)
    file(COPY ${ENGINE_SHADERS} DESTINATION  ${CMAKE_CURRENT_BINARY_DIR}/demo/shaders/)
    add_subdirectory(demo)
endif()