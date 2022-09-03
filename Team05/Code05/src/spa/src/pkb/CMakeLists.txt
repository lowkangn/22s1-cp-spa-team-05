file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
add_library(pkb ${srcs} ${headers})
# this makes the headers publicly accessible for other projects which uses sp lib
target_include_directories(pkb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/interfaces)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/design_objects)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/table_managers)