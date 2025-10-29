#version 450 core

in vec3 vt_pos;

out vec4 frag_color;

uniform samplerCube environment_map;

void main()
{
    frag_color = texture(environment_map, vt_pos);    
}
