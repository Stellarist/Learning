#version 450 core
    
layout(points) in;
layout(line_strip, max_vertices = 256) out;

uniform int counts;
uniform int segments;

layout(std140, binding = 0) uniform ControlPoints {
    vec4 control_points[16];
};

float binomial(int n, int k)
{
    if(k == 0 || k == n)
        return 1.0;
    if(k == 1 || k == n - 1)
        return float(n);
    
    float res = 1.0;
    for(int j = 0; j < k; j++)
        res *= float(n - j) / float(j + 1);

    return res;
}

vec2 bezier(float t)
{
    vec2 p = vec2(0.0);
    int n = counts - 1;

    for(int i = 0; i < counts; i++) {
        float b = binomial(n, i) * pow(t, float(i)) * pow(1.0 - t, float(n - i));
        p += b * control_points[i].xy;
    }

    return p;
}

void main()
{
    if(counts < 2)
        return;

    for(int i = 0; i <= segments; i++) {
        float t = float(i) / float(segments);
        vec2 curve_point = bezier(t);

        gl_Position = vec4(curve_point, 0.0, 1.0);
        EmitVertex();
    }

    EndPrimitive();
}
