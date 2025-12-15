cmake_minimum_required(VERSION 4.0)

project(Canvas)

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader REQUIRED)

add_subdirectory(rasterizer)
add_subdirectory(raytracer)
