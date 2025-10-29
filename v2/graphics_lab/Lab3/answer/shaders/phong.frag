#version 450 core

in vec3 vertex_pos;
in vec3 vertex_normal;

out vec4 frag_color;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 object_color;
uniform vec3 view_pos;

uniform float ambient_strength= 0.1;
uniform float specular_strength = 0.5;
uniform float shininess = 64;

void main()
{
    vec3 ambient = ambient_strength * light_color;

    vec3 norm = normalize(vertex_normal);
    vec3 light_dir = normalize(light_pos -  vertex_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 view_dir = normalize(view_pos - vertex_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    vec3 specular = specular_strength * spec * light_color;

    vec3 result = (ambient + diffuse + specular) * object_color;
    frag_color = vec4(result, 1.0);
}
