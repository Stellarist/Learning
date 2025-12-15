#version 450 core

in vec3 world_pos;

out vec4 frag_color;

uniform bool color_mode;

uniform vec3 light_dir;
uniform vec3 light_color;
uniform vec3 object_color;

void main()
{
    vec3 normal = normalize(cross(dFdx(world_pos), dFdy(world_pos)));
    vec3 direction = -normalize(light_dir);
    float diffuse = max(dot(normal, direction), 0.0);
    vec3 base_color = color_mode ? normal : object_color;

    frag_color = vec4(diffuse * light_color * base_color, 1.0);
}
