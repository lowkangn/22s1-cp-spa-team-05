file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
add_library(sp ${srcs} ${headers})
# this makes the headers publicarlly accessible for other projects which uses sp lib
target_include_directories(sp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/design_extractor)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lexer)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/parser)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dataclasses)
