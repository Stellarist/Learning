cmake_minimum_required(VERSION 3.10)

project(lab3)

add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}")
add_compile_definitions(SHADER_PATH="${CMAKE_SOURCE_DIR}/shaders")

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

set(LIB_LIST
    glad::glad
    glfw
    glm::glm
)

add_executable(sphere 
    src/sphere.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Light.cpp
)

add_executable(sphere2
    src/sphere2.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Light.cpp
)

target_link_libraries(sphere ${LIB_LIST})
target_link_libraries(sphere2 ${LIB_LIST})
