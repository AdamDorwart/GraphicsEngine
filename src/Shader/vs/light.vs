#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in vec3 Color;
layout (location = 4) in float Visible;

uniform mat4 gWVP;
uniform mat4 gDepthWVP;
uniform mat4 gWorld;

out vec3 NormalFS;
out vec3 WorldPos;
out vec3 ShadowCoord;
out vec2 TexCoordFS;

void main () {
	//Visible = Visible;
	TexCoordFS = TexCoord;
	ShadowCoord = (gDepthWVP * vec4(Position, 1.0)).xyz;
	gl_Position = gWVP * vec4(Position, 1.0);
	NormalFS = (gWorld * vec4(Normal, 0.0)).xyz;
	WorldPos = (gWorld * vec4(Position, 1.0)).xyz;
}