#pragma once
#include <glm/glm.hpp>
#include "Shader.h"

using namespace glm;

class SimpleShader : public Shader {
	public:
		SimpleShader();
		virtual ~SimpleShader();

		virtual bool init();

		void setWVP(mat4 wvp);
		void setW(mat4 w);
	private:

		GLint m_WVPLocation;
		GLint m_WorldLocation;
};