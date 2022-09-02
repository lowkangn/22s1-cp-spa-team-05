file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
#add_library(spa ${srcs} ${headers})
# this makes the headers publicly accessible for other projects which uses sp lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/interfaces)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/design_objects)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/table_managers)