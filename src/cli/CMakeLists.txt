# 编译期嵌入主题文件
set(EMBED_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/embedded_theme.cpp)
add_custom_command(
    OUTPUT ${EMBED_OUTPUT}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/embed_theme.py
            ${CMAKE_SOURCE_DIR}
            ${EMBED_OUTPUT}
    DEPENDS ${CMAKE_SOURCE_DIR}/scripts/embed_theme.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Embedding theme files..."
)
string(TIMESTAMP CMAKE_CURRENT_DATE "%Y-%m-%d")

file(GLOB_RECURSE CURRENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(APPEND CURRENT_SOURCES ${EMBED_OUTPUT})

add_library(cli STATIC ${CURRENT_SOURCES})
target_include_directories(cli PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(cli PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(cli PUBLIC builder CLI11::CLI11)
