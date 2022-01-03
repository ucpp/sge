project(Demo)

add_executable(${PROJECT_NAME} main.cpp application.cpp application.h)

add_custom_command(
        TARGET Demo PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/resources/
                ${CMAKE_CURRENT_BINARY_DIR}/resources/)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad imgui engine ${OPENGL_LIBRARIES})