# 匹配所有cpp文件
file(GLOB_RECURSE CURRENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 构建静态库
add_library(cli STATIC ${CURRENT_SOURCES})