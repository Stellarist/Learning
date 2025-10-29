#version 450 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

out vec3 vertex_color;

uniform mat4 model;

void main()
{
    // TODO: Set the position of the vertex
    vertex_color = a_color;
}
