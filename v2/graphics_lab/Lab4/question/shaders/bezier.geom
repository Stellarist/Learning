#version 450 core
    
layout(points) in;
layout(line_strip, max_vertices = 256) out;

uniform int counts;
uniform int segments;

layout(std140, binding = 0) uniform ControlPoints {
    vec4 control_points[16];
};

// TODO: complete the binomial coefficient function
float binomial(int n, int k)
{
}

// TODO: implement the Bezier curve calculation
vec2 bezier(float t)
{
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
