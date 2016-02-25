#pragma once
#include <glm/glm.hpp>
#include "Shader.h"

using namespace glm;

class FlatShader : public Shader {
	public:
		FlatShader();
		virtual ~FlatShader();

		virtual bool init();

		void setWVP(mat4 wvp);
		void setW(mat4 w);
	private:

		GLint m_WVPLocation;
		GLint m_WorldLocation;
};