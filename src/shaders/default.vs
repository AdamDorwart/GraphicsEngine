#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Color;

out vec3 Color0;

void main () {
	Color0 = Color;
	gl_Position = vec4 (Position, 1.0);
}