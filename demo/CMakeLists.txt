project(Demo)

add_executable(${PROJECT_NAME} main.cpp application.cpp application.h)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad imgui engine ${OPENGL_LIBRARIES})