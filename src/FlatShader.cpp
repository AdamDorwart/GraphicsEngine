#include "FlatShader.h"
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

FlatShader::FlatShader() {

}

FlatShader::~FlatShader() {

}

bool FlatShader::init() {
	if (!Shader::init()) {
		LOG_ERR("Unable to initialize shader.\n");
		return false;
	}

	if (!addShader(GL_VERTEX_SHADER, "simple.vs")) {
		LOG_ERR("Unable to add vertex shader: simple.vs\n");
		return false;
	}

	if (!addShader(GL_GEOMETRY_SHADER, "simple.gs")) {
		LOG_ERR("Unable to add geometry shader: simple.gs\n");
		return false;
	}

	if (!addShader(GL_FRAGMENT_SHADER, "flat.fs")) {
		LOG_ERR("Unable to add fragment shader: simple.fs\n");
		return false;
	}

	if (!finalize()) {
		LOG_ERR("Unable to finalize shader.\n");
		return false;
	}

	m_WVPLocation = getUniformLocation("gWVP");
	m_WorldLocation = getUniformLocation("gWorld");

	return true;
}

void FlatShader::setWVP(mat4 WVP) {
	glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, value_ptr(WVP));
}


void FlatShader::setW(mat4 W) {
	glUniformMatrix4fv(m_WorldLocation, 1, GL_FALSE, value_ptr(W));
}
