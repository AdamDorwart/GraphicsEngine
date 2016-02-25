#pragma once
#include <glm/glm.hpp>
#include "SceneGraph.h"
#include "CoordFrame.h"
#include "FlatShader.h"
#include "NormalShader.h"

using namespace glm;

class RenderPipeline : public CoordFrameListener {
	private:
		FlatShader* m_flatShader;
		NormalShader* m_normalShader;
		CoordFrame* m_coordFrame;

		bool m_useFlatShading;
	public:
		RenderPipeline();
		~RenderPipeline();

		bool init();

		void render(vec2 dim, SceneGraph* scene);

		virtual void consume(CoordFrame* frame);

		CoordFrame* getCoordFrame();

		void setFlatShading(bool val);
};