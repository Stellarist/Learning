#version 450 core

in vec3 vt_pos;
in vec3 vt_normal;
in vec4 vt_light_space_pos;

out vec4 frag_color;

struct Light {
    vec3 direction;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
};

uniform Light light;
uniform vec3 camera_pos;
uniform vec3 object_color;

uniform bool PCF_or_PCSS;    // true: PCF, false: PCSS
uniform float light_size;     // Size of the light source
uniform int blocker_search_radius; // Radius for searching blockers
uniform int shadow_sampling_radius; // Radius for shadow sampling
uniform sampler2D shadow_map;  // Shadow map texture

// TODO: Complete PCF function
float PCF(vec4 light_space_pos)
{
    // Calculate the projected coordinates

    // Discard fragments that are outside the light's view

    // Perform PCF sampling
}

// TODO: Complete PCSS function
float PCSS(vec4 light_space_pos)
{
    // Calculate the projected coordinates
    
    // Discard fragments that are outside the light's view

    // Perform PCSS sampling
}

void main()
{
    vec3 ambient = light.ambient * light.color;

    vec3 normal = normalize(vt_normal);
    vec3 light_dir = -normalize(light.direction);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * light.diffuse * light.color;

    vec3 view_dir = normalize(camera_pos - vt_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 64);
    vec3 specular = spec * light.specular * light.color;

    float shadow = PCF_or_PCSS ? PCF(vt_light_space_pos) : PCSS(vt_light_space_pos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * object_color;

    frag_color = vec4(lighting, 1.0);
}
