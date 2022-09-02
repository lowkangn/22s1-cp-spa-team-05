file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
add_library(qps ${srcs} ${headers})
# this makes the headers publicly accessible for other projects which uses qps lib
target_include_directories(qps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/query)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/query_parser)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/query_evaluator)
