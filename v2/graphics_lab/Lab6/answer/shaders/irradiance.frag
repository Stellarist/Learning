#version 450 core

in vec3 vt_pos;
in vec2 vt_texcoord;

out vec4 frag_color;

uniform samplerCube environment_map;

const float PI = 3.14159265359;

void main()
{
    vec3 N = normalize(vt_pos);
    vec3 irradiance = vec3(0.0);

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));
       
    float sample_data = 0.025;
    float nr_samples = 0.0;

    for(float phi = 0.0; phi < 2.0 * PI; phi += sample_data) {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sample_data) {
            vec3 tangent_sample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * N; 

            irradiance += texture(environment_map, sample_vec).rgb * cos(theta) * sin(theta);
            nr_samples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nr_samples));
    frag_color = vec4(irradiance, 1.0);
}
