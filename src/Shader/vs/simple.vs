#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Color;
layout (location = 3) in float Visible;

uniform mat4 gWVP;
uniform mat4 gWorld;

// To Geometry Shader
out VS_OUT {
    float Visible;
    flat vec3 Color;
    vec3 Normal;
    vec3 WorldPos;
} vs_out;

void main () {
	vs_out.Visible = Visible;
	vs_out.Color = Color;
	gl_Position = gWVP * vec4(Position, 1.0);
	vs_out.Normal = (gWorld * vec4(Normal, 0.0)).xyz;
	vs_out.WorldPos = (gWorld * vec4(Position, 1.0)).xyz;
}