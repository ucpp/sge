cmake_minimum_required(VERSION 3.14)

project(sge_engine)

# C++ standard configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(UNICODE)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(EXTERNALS_PATH externals)
set(PIX_EVENT_RUNTIME_DLL ${CMAKE_CURRENT_SOURCE_DIR}/externals/pix/bin/WinPixEventRuntime.dll)

# Build options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_SAMPLES "Build samples" ON)

# Add external dependencies
add_subdirectory(${EXTERNALS_PATH}/imgui)
add_subdirectory(${EXTERNALS_PATH}/assimp)
add_subdirectory(${EXTERNALS_PATH}/directx_tex)
add_subdirectory(${EXTERNALS_PATH}/freetype)


add_subdirectory(engine)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

if(BUILD_TESTS)
    set(GOOGLETEST_VERSION 1.14.0)
    add_subdirectory(${EXTERNALS_PATH}/googletest)
    add_subdirectory(tests)
endif()