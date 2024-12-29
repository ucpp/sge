project(sge VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(UNICODE)

set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../externals)
set(ENGINE_HEADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public)
set(ENGINE_SOURCES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private)

file(GLOB_RECURSE ENGINE_HEADERS ${ENGINE_HEADERS_PATH}/*.h)
file(GLOB_RECURSE ENGINE_SOURCES ${ENGINE_SOURCES_PATH}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${ENGINE_HEADERS} ${ENGINE_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC assimp imgui d3d12 dxgi dxguid d3dcompiler DirectXTex)
target_include_directories(
    ${PROJECT_NAME} PUBLIC  
    ${ENGINE_HEADERS_PATH}  
    ${THIRD_PARTY_PATH}/imgui/  
    ${THIRD_PARTY_PATH}/simple_math/  
    ${THIRD_PARTY_PATH}/directx_tex/DirectXTex/  
)