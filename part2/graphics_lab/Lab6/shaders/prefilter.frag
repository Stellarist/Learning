#version 450 core

in vec3 vt_pos;

out vec4 frag_color;

uniform float roughness;
uniform samplerCube environment_map;

const float PI = 3.14159265359;

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness * roughness;
	float phi = 2.0 * PI * Xi.x;
	float cos_theta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
	float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	vec3 H = vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
	vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	vec3 sample_vec = tangent * H.x + bitangent * H.y + N * H.z;

	return normalize(sample_vec);
}

vec2 hammersley(uint i, uint N)
{
    uint bits = i;
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float radical_inverse = float(bits) * 2.3283064365386963e-10;
	return vec2(float(i)/float(N), radical_inverse);
}

void main()
{
    vec3 N = normalize(vt_pos);
    vec3 R = N;
    vec3 V = R;

    int sample_count = 1024;
    vec3 prefiltered = vec3(0.0);
    float total_weight = 0.0;

    for(int i = 0; i < sample_count; i++) {
        vec2 Xi = hammersley(i, sample_count);
        vec3 H = importanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0) {
            float D = distributionGGX(N, H, roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

            float resolution = 512.0;
            float sa_texel  = 4.0 * PI / (6.0 * resolution * resolution);
            float sa_sample = 1.0 / (float(sample_count) * pdf + 0.0001);
            float mip_level = (roughness == 0.0) ? 0.0 : 0.5 * log2(sa_sample / sa_texel);

            prefiltered += textureLod(environment_map, L, mip_level).rgb * NdotL;
            total_weight += NdotL;
        }
    }

    prefiltered = prefiltered / total_weight;
    frag_color = vec4(prefiltered, 1.0);
}
