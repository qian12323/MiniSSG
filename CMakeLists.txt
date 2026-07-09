cmake_minimum_required(VERSION 3.20)
project(miniSSG
  VERSION 0.1.0
  DESCRIPTION "A minimal static site generator for personal blog"
  LANGUAGES CXX
)

# C++ 标准配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 需要 Python3 用于编译时嵌入主题文件
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# 产物输出目录
# 可执行文件放 build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# 静态/动态库放 build/lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) 

# 编译开关：规范英文描述
option(SSG_BUILD_CLI "Build CLI executable with main entry, OFF only build static libraries" ON)

# 全局头文件路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加子目录
add_subdirectory(external)
add_subdirectory(src)

