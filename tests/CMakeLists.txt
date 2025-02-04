project(tests)

add_executable(${PROJECT_NAME} sge_math_tests.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        sge  
        gtest  
        gtest_main  
)

set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../externals)
target_include_directories(${PROJECT_NAME} PRIVATE ${THIRD_PARTY_PATH}/googletest/include/)

enable_testing()
add_test(NAME sge_math_tests COMMAND ${PROJECT_NAME})
