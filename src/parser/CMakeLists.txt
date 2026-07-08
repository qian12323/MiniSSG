# 匹配所有cpp文件
file(GLOB_RECURSE CURRENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 构建静态库
add_library(parser STATIC ${CURRENT_SOURCES})

# 链接一下第三方库
target_include_directories(parser PUBLIC ${MD4C_SOURCE_DIR}/src)
target_link_libraries(parser PUBLIC md4c-html yaml-cpp)