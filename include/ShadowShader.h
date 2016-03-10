#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "CoordFrame.h"

using namespace glm;

class ShadowShader : public Shader, public CoordFrameListener {
	public:
		ShadowShader();
		virtual ~ShadowShader();

		virtual bool init();

		void setWVP(mat4 wvp);

		virtual void consume(CoordFrame* frame);

	private:
		GLint m_WVPLocation;
};