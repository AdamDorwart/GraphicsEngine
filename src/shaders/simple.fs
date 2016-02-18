#version 330
// From Geometry Shader
flat in vec3 Color;
in vec3 Normal;
in vec3 WorldPos;

// To Framebuffer
out vec4 frag_color;

void main () {
	frag_color = vec4(Color, 1.0);
}