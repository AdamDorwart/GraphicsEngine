#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "CoordFrame.h"

using namespace glm;

class NormalShader : public Shader, public CoordFrameListener {
	public:
		NormalShader();
		virtual ~NormalShader();

		virtual bool init();

		void setWVP(mat4 wvp);
		void setW(mat4 w);

		virtual void consume(CoordFrame* frame);
	private:

		GLint m_WVPLocation;
		GLint m_WorldLocation;
};