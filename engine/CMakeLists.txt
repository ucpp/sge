project(engine)

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

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/demo/shaders/)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/demo/shaders/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ DESTINATION ${CMAKE_BINARY_DIR}/demo/shaders/)

target_link_libraries(engine PUBLIC glad glfw glm imgui assimp ${OPENGL_LIBRARIES})
target_include_directories(engine PUBLIC include ../externals/stb/)
