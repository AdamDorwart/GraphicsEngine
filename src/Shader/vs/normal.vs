#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;
layout (location = 3) in vec3 Bitangent;
layout (location = 4) in vec2 TexCoord;
layout (location = 5) in float Visible;

out VS_OUT {
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
} vs_out;

void main () {
	gl_Position = vec4(Position, 1.0);
	vs_out.Normal = Normal;
	vs_out.Tangent = Tangent;
	vs_out.Bitangent = Bitangent;
}