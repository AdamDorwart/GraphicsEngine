#version 330

in vec3 NormalFS;
in vec3 WorldPos;
in vec3 ShadowCoord;
in vec2 TexCoordFS;

uniform sampler2D Texture0;

// To Framebuffer
out vec4 frag_color;

void main () {
	float visibility = 1.0;
	if ( texture( Texture0, ShadowCoord.xy ).z < ShadowCoord.z){
		visibility = 0.5;
	}
	//frag_color = vec4(visibility*vec3(.5,.5,.5), 1.0);
	float depth = texture(Texture0, TexCoordFS).z;
	frag_color = vec4(vec3(.1+depth,.1+depth,.1+depth), 1.0);
}