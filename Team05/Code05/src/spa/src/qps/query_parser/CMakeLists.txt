file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
add_library(query_parser ${srcs} ${headers})
# this makes the headers publicly accessible for other projects which uses qps lib
target_include_directories(query_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
