#version 330
layout (triangles) in;
layout (line_strip, max_vertices = 18) out;

uniform mat4 gWVP;
uniform mat4 gWorld;

// From Vertex Shader
in VS_OUT {
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
} gs_in[];

// To Fragment Shader
out vec3 Color;

float length = 0.05;

void main() {
	for (int i = 0; i < gs_in.length(); i++) {
		vec3 P = gl_in[i].gl_Position.xyz;

		vec3 N = normalize(gs_in[i].Normal);
		gl_Position = gWVP * vec4(P, 1.0);
		Color = vec3(1, 0, 0);
		EmitVertex();
		gl_Position = gWVP * vec4(P + N*length, 1.0);
		Color = vec3(1, 0, 0);
		EmitVertex();

		EndPrimitive();

		vec3 T = normalize(gs_in[i].Tangent);
		gl_Position = gWVP * vec4(P, 1.0);
		Color = vec3(0, 1, 0);
		EmitVertex();
		gl_Position = gWVP * vec4(P + T*length, 1.0);
		Color = vec3(0, 1, 0);
		EmitVertex();

		EndPrimitive();

		vec3 B = normalize(gs_in[i].Bitangent);
		gl_Position = gWVP * vec4(P, 1.0);
		Color = vec3(0, 0, 1);
		EmitVertex();
		gl_Position = gWVP * vec4(P + B*length, 1.0);
		Color = vec3(0, 0, 1);
		EmitVertex();

		EndPrimitive();
	}
}