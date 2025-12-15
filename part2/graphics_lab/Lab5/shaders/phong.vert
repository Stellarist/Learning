#version 450 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

out vec3 vt_pos;
out vec3 vt_normal;
out vec2 vt_texcoord;
out vec4 vt_light_space_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space;

void main()
{
    vt_pos = vec3(model * vec4(in_pos, 1.0));
    vt_normal = transpose(inverse(mat3(model))) * in_normal;
    vt_light_space_pos = light_space * vec4(vt_pos, 1.0);
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
}
