file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
add_library(parser_exceptions ${srcs} ${headers})
# this makes the headers publicarlly accessible for other projects which uses sp lib
target_include_directories(parser_exceptions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


