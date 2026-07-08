file(GLOB_RECURSE CURRENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(renderer STATIC ${CURRENT_SOURCES})
target_include_directories(renderer PUBLIC ${CMAKE_SOURCE_DIR}/include)
