# Project Demo
project(demo)

# Create executable
add_executable(${PROJECT_NAME} demo.cpp)

# Link dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC sge)

# Copy resources and shaders directories
add_custom_command(
    TARGET ${PROJECT_NAME} 
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/ 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_CURRENT_SOURCE_DIR}/../engine/shaders/ 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/
)

# Copy PIX runtime DLL
add_custom_command(
    TARGET ${PROJECT_NAME} 
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${PIX_EVENT_RUNTIME_DLL} 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)