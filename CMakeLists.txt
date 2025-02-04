cmake_minimum_required(VERSION 3.14)

project(sge_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(EXTERNALS_PATH externals)
add_subdirectory(${EXTERNALS_PATH}/imgui)
add_subdirectory(${EXTERNALS_PATH}/assimp)
add_subdirectory(${EXTERNALS_PATH}/directx_tex)

add_subdirectory(engine)
add_subdirectory(samples)

set(GOOGLETEST_VERSION 1.14.0)
add_subdirectory(${EXTERNALS_PATH}/googletest)
add_subdirectory(tests)