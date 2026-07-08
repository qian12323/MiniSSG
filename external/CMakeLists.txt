include(FetchContent)

# 
FetchContent_Declare(
  md4c
  GIT_REPOSITORY https://github.com/mity/md4c.git
  GIT_TAG release-0.5.2
  GIT_SHALLOW TRUE
)
set(MD4C_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MD4C_BUILD_TESTING  OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(md4c)

# yaml-cpp
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0
  GIT_SHALLOW TRUE
)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(yaml-cpp)

# gtest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
  GIT_SHALLOW TRUE
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# CLI11 - command line parsing
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v2.4.2
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(cli11)
