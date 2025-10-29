cmake_minimum_required(VERSION 4.0)

project(raytracer)

add_compile_definitions(PROJECT_PATH_2="${CMAKE_SOURCE_DIR}")
add_compile_definitions(BUILD_PATH_2="${CMAKE_BINARY_DIR}")

file(GLOB_RECURSE INC_LIST src/*.hpp)
file(GLOB_RECURSE SRC_LIST src/*.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${INC_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SRC_LIST})

add_executable(raytracer
    ${INC_LIST} 
    ${SRC_LIST}
)

target_link_libraries(raytracer
    Eigen3::Eigen
    tinyobjloader::tinyobjloader
    ${STB_LIBRARIES}
)

