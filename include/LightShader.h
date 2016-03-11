#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "CoordFrame.h"

using namespace glm;

class LightShader : public Shader, public CoordFrameListener {
	public:
		LightShader();
		virtual ~LightShader();

		virtual bool init();

		void setWVP(mat4 wvp);
		void setDepthWVP(mat4 depthMVP);
		void setW(mat4 w);
		void setLightPos(vec3 pos);
		void setViewPos(vec3 pos);

		virtual void consume(CoordFrame* frame);
	private:

		GLint m_DepthWVPLocation;
		GLint m_WVPLocation;
		GLint m_WorldLocation;
		GLint m_LightPosLocation;
		GLint m_ViewPosLocation;
};