file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers} "src/pkb/design_objects/relationships/PkbRelationship.h")
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# these add the sub folders to the build path
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/sp)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/pkb)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/qps)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/common)
