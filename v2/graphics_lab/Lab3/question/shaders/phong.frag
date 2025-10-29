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
    //TODO: compute ambient, diffuse, and specular components to realize Phong shading
}
