#pragma once
#include <glm/glm.hpp>
#include "SceneGraph.h"
#include "CoordFrame.h"
#include "SimpleShader.h"

using namespace glm;

class RenderPipeline : public CoordFrameListener {
	private:
		SimpleShader* m_shader;
		CoordFrame* m_coordFrame;
	public:
		RenderPipeline();
		~RenderPipeline();

		bool init();

		void render(vec2 dim, SceneGraph* scene);

		virtual void consume(CoordFrame* frame);

		CoordFrame* getCoordFrame();
};