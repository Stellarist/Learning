#version 450 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texcoords;

out vec2 vt_texcoords;

void main()
{
    gl_Position = vec4(in_pos, 1.0);
    vt_texcoords = in_texcoords;
}
