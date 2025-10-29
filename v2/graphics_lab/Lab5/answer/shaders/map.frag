#version 450 core

in vec2 vt_texcoords;

out vec4 frag_color;

uniform float near_plane;
uniform float far_plane;
uniform sampler2D shadow_map;

void main()
{
    float depth = texture(shadow_map, vt_texcoords).r;
    frag_color = vec4(vec3(depth), 1.0);
}
