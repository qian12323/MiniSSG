# 具体的每个模块的编译链接放到各自的cmakelists中去
# 添加子模块
add_subdirectory(core)
add_subdirectory(parser)
add_subdirectory(renderer)
add_subdirectory(builder)
add_subdirectory(cli)

# 将各个组件链接成一个lib
add_library(ssg_all INTERFACE)
target_link_libraries(ssg_all
  INTERFACE 
  # core
  parser 
  # renderer 
  # builder 
  cli
)

if(SSG_BUILD_CLI)
  add_executable(minissg main.cpp)
  target_link_libraries(minissg PRIVATE ssg_all)
endif()
