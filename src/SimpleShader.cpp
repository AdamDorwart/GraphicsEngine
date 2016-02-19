#include "SimpleShader.h"
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

SimpleShader::SimpleShader() {

}

SimpleShader::~SimpleShader() {

}

bool SimpleShader::init() {
	if (!Shader::init()) {
		Logger::err("Unable to initialize shader.\n");
		return false;
	}

	if (!addShader(GL_VERTEX_SHADER, "simple.vs")) {
		Logger::err("Unable to add vertex shader: simple.vs\n");
		return false;
	}

	if (!addShader(GL_GEOMETRY_SHADER, "simple.gs")) {
		Logger::err("Unable to add geometry shader: simple.gs\n");
		return false;
	}

	if (!addShader(GL_FRAGMENT_SHADER, "simple.fs")) {
		Logger::err("Unable to add fragment shader: simple.fs\n");
		return false;
	}

	if (!finalize()) {
		Logger::err("Unable to finalize shader.\n");
		return false;
	}

	m_WVPLocation = getUniformLocation("gWVP");
	m_WorldLocation = getUniformLocation("gWorld");

	return true;
}

void SimpleShader::setWVP(mat4 WVP) {
	glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, value_ptr(WVP));
}


void SimpleShader::setW(mat4 W) {
	glUniformMatrix4fv(m_WorldLocation, 1, GL_FALSE, value_ptr(W));
}
