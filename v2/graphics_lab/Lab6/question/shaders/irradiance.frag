#version 450 core

in vec3 vt_pos;
in vec2 vt_texcoord;

out vec4 frag_color;

uniform samplerCube environment_map;

const float PI = 3.14159265359;

// TODO: Compute the irradiance environment map
void main()
{
}
