#include "ShadowShader.h"
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

ShadowShader::ShadowShader() {

}

ShadowShader::~ShadowShader() {

}

bool ShadowShader::init() {
	if (!Shader::init()) {
		LogError("Unable to initialize shader.\n");
		return false;
	}

	if (!addShader(GL_VERTEX_SHADER, "vs/position.vs")) {
		LogError("Unable to add vertex shader: position.vs\n");
		return false;
	}

	if (!addShader(GL_FRAGMENT_SHADER, "fs/empty.fs")) {
		LogError("Unable to add fragment shader: empty.fs\n");
		return false;
	}

	if (!finalize()) {
		LogError("Unable to finalize shader.\n");
		return false;
	}

	m_WVPLocation = getUniformLocation("gWVP");

	return true;
}

void ShadowShader::setWVP(mat4 WVP) {
	glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, value_ptr(WVP));
}

void ShadowShader::consume(CoordFrame* frame) {
	setWVP(frame->getPCW());
}
