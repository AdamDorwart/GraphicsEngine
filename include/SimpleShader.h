#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "CoordFrame.h"

using namespace glm;

class SimpleShader : public Shader, public CoordFrameListener {
	public:
		SimpleShader(CoordFrame* frame);
		~SimpleShader();

		virtual bool init();

		virtual void consume(CoordFrame* frame);
	private:
		void setWVP(mat4 wvp);
		void setW(mat4 w);

		GLint m_WVPLocation;
		GLint m_WorldLocation;

		CoordFrame* m_frame;
};