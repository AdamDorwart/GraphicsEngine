#version 330
in vec3 Color0;
out vec4 frag_color;
void main () {
	frag_color = vec4 (Color0, 1.0);
}