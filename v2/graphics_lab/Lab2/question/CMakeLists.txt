cmake_minimum_required(VERSION 3.10)

project(lab2)

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

add_executable(pyramid 
    src/pyramid.cpp
    src/Camera.cpp
    src/Shader.cpp
)

add_executable(cube
    src/cube.cpp
    src/Camera.cpp
    src/Shader.cpp
)

target_link_libraries(pyramid ${LIB_LIST})
target_link_libraries(cube ${LIB_LIST})
