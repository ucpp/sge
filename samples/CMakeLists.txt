project(demo)

add_executable(${PROJECT_NAME} demo.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC sge)

add_custom_command(TARGET ${PROJECT_NAME} 
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/../engine/shaders/ 
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/)