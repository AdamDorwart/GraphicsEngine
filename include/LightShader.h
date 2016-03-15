#pragma once
#include <glm/glm.hpp>
#include "MaterialShader.h"
#include "CoordFrame.h"
#include "CubeMap.h"
#include "Texture.h"

using namespace glm;

class LightShader : public MaterialShader, public CoordFrameListener {
	public:
		enum {CUBEMAP_UNIT = MaterialShader::NEXT_AVIL_UNIT, SHADOWMAP_UNIT};
		LightShader();
		virtual ~LightShader();

		virtual bool init();

		void setWVP(mat4 wvp);
		void setDepthWVP(mat4 depthMVP);
		void setW(mat4 w);
		void setLightPos(vec3 pos);
		void setViewPos(vec3 pos);
		void setCubeMap(CubeMap* cubeMap);
		void setShadowMap(Texture* shadowMap);

		virtual void consume(CoordFrame* frame);
	private:

		GLint m_DepthWVPLocation;
		GLint m_WVPLocation;
		GLint m_WorldLocation;
		GLint m_LightPosLocation;
		GLint m_ViewPosLocation;
		GLint m_CubeMapLocation;
		GLint m_ShadowMapLocation;
};