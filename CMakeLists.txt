cmake_minimum_required(VERSION 3.10)
project(ogl_render VERSION 0.1)

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
    # copy shaders from engine
    file(GLOB ENGINE_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/engine/shaders/*.*)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/demo/shaders/)
    file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/demo/shaders/)
    file(COPY ${ENGINE_SHADERS} DESTINATION  ${CMAKE_CURRENT_BINARY_DIR}/demo/shaders/)

    # copy resources from demo
    file(GLOB DEMO_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/demo/resources/*.*)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/)
    file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/)
    file(COPY ${DEMO_RESOURCES} DESTINATION  ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/)

    add_subdirectory(demo)
endif()