/* This is a simple shader that just transforms the verticies
 * into screen space and passes them to the fragment shader.
 * Used in shadow maps
 */
#version 330

layout (location = 0) in vec3 Position;

uniform mat4 gWVP;

void main() {
	gl_Position = gWVP * vec4(Position, 1.0);
}