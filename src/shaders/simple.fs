#version 330
flat in vec3 Color0;
in vec3 Normal0;
in vec3 WorldPos0;

out vec4 frag_color;

void main () {
	frag_color = vec4(Color0, 1.0);
}