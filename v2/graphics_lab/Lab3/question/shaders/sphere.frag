#version 450 core

in vec3 vertex_pos;
in vec3 vertex_normal;

out vec4 frag_color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outer_cutoff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

uniform DirLight dir_light;
uniform PointLight point_light;
uniform SpotLight spot_light;

uniform vec3 object_color;
uniform vec3 view_pos;
uniform float shininess = 64.0;

vec3 calDirLight(DirLight light, vec3 normal, vec3 view_dir);
vec3 calPointLight(PointLight light, vec3 normal, vec3 vertex_pos, vec3 view_dir);
vec3 calSpotLight(SpotLight light, vec3 normal, vec3 vertex_pos, vec3 view_dir);

void main()
{    
    vec3 normal = normalize(vertex_normal);
    vec3 view_dir = normalize(view_pos - vertex_pos);
    
    vec3 color = calDirLight(dir_light, normal, view_dir) +
        calPointLight(point_light, normal, vertex_pos, view_dir) +
        calSpotLight(spot_light, normal, vertex_pos, view_dir);

    frag_color = vec4(clamp(color, 0.0, 1.0), 1.0);
}

vec3 calDirLight(DirLight light, vec3 normal, vec3 view_dir)
{
    // TODO: compute ambient, diffuse, and specular components for directional light
}

vec3 calPointLight(PointLight light, vec3 normal, vec3 vertex_pos, vec3 view_dir)
{
    // TODO: compute ambient, diffuse, and specular components for point light
}

vec3 calSpotLight(SpotLight light, vec3 normal, vec3 vertex_pos, vec3 view_dir)
{
    // TODO: compute ambient, diffuse, and specular components for spot light
}
