file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/entities)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/relationships)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/patterns)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/graphs)



