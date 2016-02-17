#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Color;

uniform mat4 gWVP;
uniform mat4 gWorld;

flat out vec3 Color0;
out vec3 Normal0;
out vec3 WorldPos0; 

void main () {
	Color0 = Color;
	gl_Position = gWVP * vec4(Position, 1.0);
	Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;
	WorldPos0 = (gWorld * vec4(Position, 1.0)).xyz;
}