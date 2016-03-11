#include "LightShader.h"
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

LightShader::LightShader() {

}

LightShader::~LightShader() {

}

bool LightShader::init() {
	if (!Shader::init()) {
		LogError("Unable to initialize shader.\n");
		return false;
	}

	if (!addShader(GL_VERTEX_SHADER, "vs/light.vs")) {
		LogError("Unable to add vertex shader: simple.vs\n");
		return false;
	}

	if (!addShader(GL_FRAGMENT_SHADER, "fs/light.fs")) {
		LogError("Unable to add fragment shader: simple.fs\n");
		return false;
	}

	if (!finalize()) {
		LogError("Unable to finalize shader.\n");
		return false;
	}

	m_WVPLocation = getUniformLocation("gWVP");
	m_DepthWVPLocation = getUniformLocation("gDepthWVP");
	m_WorldLocation = getUniformLocation("gWorld");
	m_LightPosLocation = getUniformLocation("gLightPos");
	m_ViewPosLocation = getUniformLocation("gViewPos");

	return true;
}

void LightShader::setWVP(mat4 WVP) {
	glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, value_ptr(WVP));
}

void LightShader::setDepthWVP(mat4 depthMVP) {
	mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);
	mat4 depthWBias = biasMatrix*depthMVP;
	glUniformMatrix4fv(m_DepthWVPLocation, 1, GL_FALSE, value_ptr(depthWBias));
}

void LightShader::setW(mat4 W) {
	glUniformMatrix4fv(m_WorldLocation, 1, GL_FALSE, value_ptr(W));
}

void LightShader::setLightPos(vec3 pos) {
	glUniform3fv(m_LightPosLocation, 1, value_ptr(pos));
}

void LightShader::setViewPos(vec3 pos) {
	glUniform3fv(m_ViewPosLocation, 1, value_ptr(pos));
}

void LightShader::consume(CoordFrame* frame) {
	setWVP(frame->getPCW());
	setW(frame->getW());
}