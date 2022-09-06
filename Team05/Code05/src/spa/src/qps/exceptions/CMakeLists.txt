file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/parsers)

# for developmental use
# add_library(query_parser ${srcs} ${headers})
# target_include_directories(query_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
