cmake_minimum_required(VERSION 3.10)

project(lab5)

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

add_executable(shadow 
    src/main.cpp
    src/Scene.cpp
    src/Shader.cpp
)

target_link_libraries(shadow ${LIB_LIST})
