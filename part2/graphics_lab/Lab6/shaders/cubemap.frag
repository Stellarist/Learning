#version 450 core

in vec3 vt_pos;

out vec4 frag_color;

uniform sampler2D environment_map;

vec2 sampleSphericalMap(vec3 v)
{
    const vec2 inv_atan = vec2(0.1591, 0.3183);
    return vec2(atan(v.z, v.x), asin(v.y)) * inv_atan + 0.5;
}

void main()
{		
    vec2 uv = sampleSphericalMap(normalize(vt_pos));
    vec3 color = pow(texture(environment_map, uv).rgb, vec3(2.2));
    
    frag_color = vec4(color, 1.0);
}
