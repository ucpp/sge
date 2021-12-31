set(ENGINE_SOURCES sources/shader.cpp sources/resource_manager.cpp)
set(ENGINE_HEADERS include/shader.h include/resource_manager.h)

add_library(engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_link_libraries(engine PUBLIC glad glfw glm ${OPENGL_LIBRARIES})
target_include_directories(engine PUBLIC include ../externals/stb/)