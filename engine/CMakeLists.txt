project(engine VERSION 0.2.0)

# core engine
set(CORE_ENGINE_SOURCES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/core/sources)
set(CORE_ENGINE_HEADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
# editor engine
set(EDITOR_ENGINE_SOURCES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/editor/sources)
set(EDITOR_ENGINE_HEADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/editor/include)
# third party
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../externals)

set(CORE_ENGINE_HEADERS 
    ${CORE_ENGINE_HEADERS_PATH}/camera.h 
    ${CORE_ENGINE_HEADERS_PATH}/config.h 
    ${CORE_ENGINE_HEADERS_PATH}/cubemap.h 
    ${CORE_ENGINE_HEADERS_PATH}/cubemap_renderer.h 
    ${CORE_ENGINE_HEADERS_PATH}/editor.h 
    ${CORE_ENGINE_HEADERS_PATH}/event.h 
    ${CORE_ENGINE_HEADERS_PATH}/imgui_renderer.h 
    ${CORE_ENGINE_HEADERS_PATH}/input_system.h 
    ${CORE_ENGINE_HEADERS_PATH}/log.h 
    ${CORE_ENGINE_HEADERS_PATH}/material.h 
    ${CORE_ENGINE_HEADERS_PATH}/mesh.h 
    ${CORE_ENGINE_HEADERS_PATH}/model.h 
    ${CORE_ENGINE_HEADERS_PATH}/render_texture.h 
    ${CORE_ENGINE_HEADERS_PATH}/renderer.h 
    ${CORE_ENGINE_HEADERS_PATH}/resource_manager.h 
    ${CORE_ENGINE_HEADERS_PATH}/scene.h 
    ${CORE_ENGINE_HEADERS_PATH}/texture.h 
    ${CORE_ENGINE_HEADERS_PATH}/utilities.h
    ${CORE_ENGINE_HEADERS_PATH}/window.h 
    ${CORE_ENGINE_HEADERS_PATH}/shader.h)

set(CORE_ENGINE_SOURCES 
    ${CORE_ENGINE_SOURCES_PATH}/camera.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/config.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/cubemap.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/cubemap_renderer.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/editor.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/imgui_renderer.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/input_system.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/log.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/material.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/mesh.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/model.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/render_texture.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/renderer.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/resource_manager.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/scene.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/texture.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/utilities.cpp
    ${CORE_ENGINE_SOURCES_PATH}/window.cpp 
    ${CORE_ENGINE_SOURCES_PATH}/shader.cpp)

set(EDITOR_ENGINE_HEADERS 
    )
set(EDITOR_ENGINE_SOURCES 
    )

add_library(engine STATIC 
    ${CORE_ENGINE_HEADERS} 
    ${CORE_ENGINE_SOURCES} 
    ${ENGINE_ENGINE_HEADERS}
    ${ENGINE_ENGINE_SOURCES})

target_link_libraries(engine PUBLIC 
    glad 
    glfw 
    glm 
    imgui 
    assimp 
    ${OPENGL_LIBRARIES})

target_include_directories(engine PUBLIC 
    ${CORE_ENGINE_HEADERS_PATH} 
    ${THIRD_PARTY_PATH}/stb/ 
    ${THIRD_PARTY_PATH}/json/)