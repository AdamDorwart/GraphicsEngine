#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;
layout (location = 3) in vec3 Bitangent;
layout (location = 4) in vec2 TexCoord;
layout (location = 5) in float Visible;

uniform mat4 gWVP;
uniform mat4 gWV;
uniform mat4 gDepthWVP;
uniform mat4 gWorld;
uniform vec3 gLightPos;
uniform vec3 gViewPos;

out VS_OUT {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 ShadowCoord;
    mat3 TBN;
} vs_out;

void main () {
	vec3 tangent = normalize(gWorld * vec4(normalize(Tangent), 0.0)).xyz;
	vec3 bitangent = normalize(gWorld * vec4(normalize(Bitangent), 0.0)).xyz;
	vec3 normal = normalize(gWorld * vec4(normalize(Normal), 0.0)).xyz;

	//Visible = Visible;
	//vs_out.Color = Color;
	vs_out.TexCoord = TexCoord;
	vs_out.ShadowCoord = gDepthWVP * gWorld * vec4(Position, 1.0);
	gl_Position = gWVP * vec4(Position, 1.0);
	vs_out.Normal = normal;
	vs_out.WorldPos = (gWorld * vec4(Position, 1.0)).xyz;

	vs_out.TBN = mat3(
		tangent,
		bitangent,
		normal
	);
}