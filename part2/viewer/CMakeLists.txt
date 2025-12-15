cmake_minimum_required(VERSION 4.0)

project(Viewer)

file(GLOB_RECURSE SRC_LIST src/*.cpp)
file(GLOB_RECURSE INC_LIST src/*.hpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SRC_LIST})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${INC_LIST})

find_package(assimp REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(Viewer 
    ${SRC_LIST} 
    ${INC_LIST}
)

target_include_directories(Viewer
	PRIVATE src
)

target_compile_definitions(Viewer
    PRIVATE
        SOURCE_DIR="${CMAKE_SOURCE_DIR}"
        SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
        GLM_ENABLE_EXPERIMENTAL
)

target_link_libraries(Viewer PRIVATE
    assimp::assimp
    glad::glad
    glfw
    glm::glm
)
