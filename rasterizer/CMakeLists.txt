cmake_minimum_required(VERSION 3.20)

project(rasterizer)

add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}")
add_compile_definitions(BUILD_RPATH="${CMAKE_BINARY_DIR}")

file(GLOB_RECURSE INC_LIST src/*.hpp)
file(GLOB_RECURSE SRC_LIST src/*.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${INC_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SRC_LIST})

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader REQUIRED)

add_executable(rasterizer 
    ${INC_LIST} 
    ${SRC_LIST}
)

target_link_libraries(rasterizer
    glfw
    glad::glad
    Eigen3::Eigen
    tinyobjloader::tinyobjloader
    ${STB_LIBRARIES}
)
