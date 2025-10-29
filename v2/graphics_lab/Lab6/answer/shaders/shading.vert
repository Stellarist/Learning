#version 450 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

out vec3 vt_pos;
out vec3 vt_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vt_pos = vec3(model * vec4(in_pos, 1.0));
    vt_normal = normalize(mat3(transpose(inverse(model))) * in_normal);
    gl_Position = projection * view * vec4(vt_pos, 1.0);
}