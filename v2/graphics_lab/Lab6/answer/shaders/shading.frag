#version 450 core

in vec3 vt_pos;
in vec3 vt_normal;

out vec4 frag_color;

uniform vec3 camera_pos;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;

uniform samplerCube irradiance_map;
uniform samplerCube prefilter_map;
uniform sampler2D brdf_lut;

vec3 fresnelSchlickRoughness(float cos_theta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 N = normalize(vt_normal);
    vec3 V = normalize(camera_pos - vt_pos);
    vec3 R = reflect(-V, N);
    float NdotV = max(dot(N, V), 0.0);
    
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    // Irradiance Map
    vec3 irradiance = texture(irradiance_map, N).rgb;
    vec3 diffuse = irradiance * albedo;

    // Prefilter Map
    float lod = roughness * 4.0;
    vec3 prefiltered = textureLod(prefilter_map, R, lod).rgb;

    // BRDF Lut
    vec2 brdf = texture(brdf_lut, vec2(NdotV, roughness)).rg;
    vec3 specular = prefiltered * (F * brdf.x + brdf.y);

    // Final Color
    vec3 color = pow(kD * diffuse + specular, vec3(1.0/2.2));
    frag_color = vec4(color, 1.0);
}
