#pragma once
#include <glm/glm.hpp>
#include "SceneGraph.h"
#include "CoordFrame.h"
#include "FrameBuffer.h"
#include "ShadowShader.h"
#include "NormalShader.h"

using namespace glm;

struct Light {
	vec3 direction;
	CoordFrame frame;
	FrameBuffer fbo;
};

class ShadowPipeline : public CoordFrameListener {
	private:
		ShadowShader* m_shadowShader;
		NormalShader* m_normalShader;
		CoordFrame* m_cameraFrame;
		std::vector<Light> m_dirLights;

	public:
		ShadowPipeline();
		~ShadowPipeline();

		bool init();

		void render(vec2 dim, SceneGraph* scene);

		virtual void consume(CoordFrame* frame);

		CoordFrame* getCameraFrame();

		void addDirLight(vec3 direction);

};