#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in vec3 Color;
layout (location = 4) in float Visible;

uniform mat4 gWVP;
uniform mat4 gDepthWVP;
uniform mat4 gWorld;

out VS_OUT {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoord;
    vec3 Color;
    vec4 ShadowCoord;
} vs_out;

void main () {
	//Visible = Visible;
	vs_out.Color = Color;
	vs_out.TexCoord = TexCoord;
	vs_out.ShadowCoord = gDepthWVP * gWorld * vec4(Position, 1.0);
	gl_Position = gWVP * vec4(Position, 1.0);
	vs_out.Normal = (gWorld * vec4(Normal, 0.0)).xyz;
	vs_out.WorldPos = (gWorld * vec4(Position, 1.0)).xyz;
}