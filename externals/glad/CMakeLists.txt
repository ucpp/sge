set(GLAD_SOURCES src/glad.c)
set(GLAD_HEADERS include/glad/glad.h include/KHR/khrplatform.h)

add_library(glad STATIC ${GLAD_SOURCES} ${GLAD_HEADERS})
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)