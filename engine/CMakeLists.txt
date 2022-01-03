set(ENGINE_SOURCES 
    sources/shader.cpp 
    sources/resource_manager.cpp 
    sources/texture.cpp 
    sources/mesh.cpp 
    sources/model.cpp 
    sources/camera.cpp)

set(ENGINE_HEADERS 
    include/shader.h 
    include/resource_manager.h 
    include/texture.h 
    include/mesh.h 
    include/model.h 
    include/camera.h)

add_library(engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_link_libraries(engine PUBLIC glad glfw glm imgui assimp ${OPENGL_LIBRARIES})
target_include_directories(engine PUBLIC include ../externals/stb/)

add_custom_command(
    TARGET engine PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
            ${CMAKE_BINARY_DIR}/Demo/shaders/)