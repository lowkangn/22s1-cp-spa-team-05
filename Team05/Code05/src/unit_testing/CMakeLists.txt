file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" )
add_executable(unit_testing ${srcs})

target_link_libraries(unit_testing spa)

# these add the sub folders to the build path
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/sp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/pkb)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/qps)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/common)