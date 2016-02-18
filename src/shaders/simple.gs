#version 330
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// From Vertex Shader
in VS_OUT {
    float Visible;
    flat vec3 Color;
    vec3 Normal;
    vec3 WorldPos;
} gs_in[];

// To Fragment Shader
flat out vec3 Color;
out vec3 Normal;
out vec3 WorldPos;

void main() {
	if (gs_in[0].Visible > 0.5 && gs_in[1].Visible > 0.5 && gs_in[2].Visible > 0.5) {
		gl_Position = gl_in[0].gl_Position;
		Color = gs_in[0].Color;
		Normal = gs_in[0].Normal;
		WorldPos = gs_in[0].WorldPos;
		EmitVertex();

		gl_Position = gl_in[1].gl_Position;
		Color = gs_in[1].Color;
		Normal = gs_in[1].Normal;
		WorldPos = gs_in[1].WorldPos;
		EmitVertex();

		gl_Position = gl_in[2].gl_Position;
		Color = gs_in[1].Color;
		Normal = gs_in[1].Normal;
		WorldPos = gs_in[1].WorldPos;
		EmitVertex();
	}
	EndPrimitive();
}