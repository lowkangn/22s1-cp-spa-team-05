file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/design_objects)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tokens)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ast)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cfg)


