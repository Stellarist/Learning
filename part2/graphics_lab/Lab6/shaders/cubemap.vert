#version 450 core

layout (location = 0) in vec3 in_pos;

out vec3 vt_pos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vt_pos = in_pos;
    gl_Position = projection * mat4(mat3(view)) * vec4(in_pos, 1.0);
}
