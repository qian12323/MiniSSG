file(GLOB_RECURSE CURRENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(builder STATIC ${CURRENT_SOURCES})
target_include_directories(builder PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(builder PUBLIC parser renderer yaml-cpp)
