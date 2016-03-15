#include "NormalShader.h"
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

NormalShader::NormalShader() {

}

NormalShader::~NormalShader() {

}

bool NormalShader::init() {
	if (!Shader::init()) {
		LogError("Unable to initialize shader.\n");
		return false;
	}

	if (!addShader(GL_VERTEX_SHADER, "vs/normal.vs")) {
		LogError("Unable to add vertex shader: normal.vs\n");
		return false;
	}

	if (!addShader(GL_GEOMETRY_SHADER, "gs/normal.gs")) {
		LogError("Unable to add geometry shader: normal.gs\n");
		return false;
	}

	if (!addShader(GL_FRAGMENT_SHADER, "fs/normal.fs")) {
		LogError("Unable to add fragment shader: normal.fs\n");
		return false;
	}

	if (!finalize()) {
		LogError("Unable to finalize shader.\n");
		return false;
	}

	m_WVPLocation = getUniformLocation("gWVP");
	m_WorldLocation = getUniformLocation("gWorld");

	return true;
}

void NormalShader::setWVP(mat4 WVP) {
	glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, value_ptr(WVP));
}


void NormalShader::setW(mat4 W) {
	glUniformMatrix4fv(m_WorldLocation, 1, GL_FALSE, value_ptr(W));
}


void NormalShader::consume(CoordFrame* frame) {
	setWVP(frame->getPCW());
	setW(frame->getW());
}