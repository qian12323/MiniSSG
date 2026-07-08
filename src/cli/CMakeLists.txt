file(GLOB_RECURSE CURRENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(cli STATIC ${CURRENT_SOURCES})
target_include_directories(cli PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(cli PUBLIC builder)
