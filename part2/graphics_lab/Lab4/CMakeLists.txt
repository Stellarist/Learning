cmake_minimum_required(VERSION 3.10)

project(lab4)

add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}")
add_compile_definitions(SHADER_PATH="${CMAKE_SOURCE_DIR}/shaders")
add_compile_definitions(ASSET_PATH="${CMAKE_SOURCE_DIR}/assets")

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)

set(LIB_LIST
    glad::glad
    glfw
    glm::glm
    imgui::imgui
)

add_executable(bezier 
    src/bezier.cpp
    src/Shader.cpp
)

add_executable(loop 
    src/loop.cpp
    src/Shader.cpp
    src/Mesh.cpp
)

target_link_libraries(bezier ${LIB_LIST})
target_link_libraries(loop ${LIB_LIST})
