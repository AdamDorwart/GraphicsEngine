#version 330
// From Geometry Shader
in vec3 Color;

// To Framebuffer
out vec4 FragColor;

void main () {
	FragColor = vec4(Color, 1.0);
}