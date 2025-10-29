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

float PCF(vec4 light_space_pos)
{
    // Calculate the projected coordinates
    vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    // Discard fragments that are outside the light's view
    float current_depth = proj_coords.z;
    if(current_depth > 1.0)
        return 0.0;

    vec3 normal = normalize(vt_normal);
    vec3 light_dir = normalize(light.direction);
    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
    vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

    // Perform PCF sampling
    float shadow = 0.0;
    for(int x = -shadow_sampling_radius; x <= shadow_sampling_radius; x++) {
        for(int y = -shadow_sampling_radius; y <= shadow_sampling_radius; y++) {
            vec2 offset = vec2(x, y) * texel_size;
            float sample_depth = texture(shadow_map, proj_coords.xy + offset).r;
            shadow += (sample_depth < current_depth - bias) ? 1.0 : 0.0;
        }
    }

    float sampling_pixels = float((2 * shadow_sampling_radius + 1) * (2 * shadow_sampling_radius + 1));
    return shadow / sampling_pixels;
}

float PCSS(vec4 light_space_pos)
{
    // Calculate the projected coordinates
    vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;
    proj_coords = proj_coords * 0.5 + 0.5;
    
    // Discard fragments that are outside the light's view
    float current_depth = proj_coords.z;
    if(current_depth > 1.0)
        return 0.0;
    
    vec3 normal = normalize(vt_normal);
    vec3 light_dir = normalize(light.direction);
    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
    vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

    // Average Blocker Depth
    int num_blockers = 0;
    float avg_blocker_depth = 0.0;
    for(int x = -blocker_search_radius; x <= blocker_search_radius; x++) {
        for(int y = -blocker_search_radius; y <= blocker_search_radius; y++) {
            vec2 offset = vec2(x, y) * texel_size;
            float sample_depth = texture(shadow_map, proj_coords.xy + offset).r;
            if(sample_depth < current_depth - bias) {
                avg_blocker_depth += sample_depth;
                num_blockers++;
            }
        }
    }
    if(num_blockers == 0)
        return 0.0;
    avg_blocker_depth /= float(num_blockers);
    
    // Calculate PCSS kernel size based on blocker distance
    float receiver_distance = current_depth;
    float penumbra_ratio = (receiver_distance - avg_blocker_depth) / avg_blocker_depth;
    float filter_radius =  penumbra_ratio * light_size;

    // Perform PCSS sampling
    float shadow = 0.0;
    for(int x = -shadow_sampling_radius; x <= shadow_sampling_radius; x++) {
        for(int y = -shadow_sampling_radius; y <= shadow_sampling_radius; y++) {
            vec2 offset = vec2(x, y) * filter_radius * texel_size;
            float sample_depth = texture(shadow_map, proj_coords.xy + offset).r;
            shadow += (sample_depth < current_depth - bias) ? 1.0 : 0.0;
        }
    }

    float sampling_pixels = float((2 * shadow_sampling_radius + 1) * (2 * shadow_sampling_radius + 1));
    return shadow / sampling_pixels;
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
