#version 330

in VS_OUT {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 ShadowCoord;
    mat3 TBN;
} fs_in;

// To Framebuffer
out vec4 frag_color;

void main () {
	frag_color = vec4(Color, 1.0);
}