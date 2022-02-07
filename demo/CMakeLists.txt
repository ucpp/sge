project(Demo)

add_executable(${PROJECT_NAME} main.cpp application.cpp application.h)

file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/resources/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/resources/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

target_link_libraries(${PROJECT_NAME} PUBLIC engine)