#include "RenderPipeline.h"

RenderPipeline::RenderPipeline() {
	m_flatShader = new FlatShader();
	m_normalShader = new NormalShader();
	m_coordFrame = new CoordFrame();
	m_coordFrame->subscribe(this);
}

RenderPipeline::~RenderPipeline() {
	delete m_flatShader;
	delete m_normalShader;
	delete m_coordFrame;
	m_coordFrame->unsubscribe(this);
}

bool RenderPipeline::init() {
	m_flatShader->init();
	m_normalShader->init();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	return true;
}

void RenderPipeline::render(vec2 dim, SceneGraph* scene) {
	m_coordFrame->setViewport(0, 0, dim.x, dim.y);


	if (m_useFlatShading) {
		m_flatShader->enable();
	} else {
		m_normalShader->enable();
	}

	m_coordFrame->resetWorldMatrix();

	scene->traverse(m_coordFrame);

	if (m_useFlatShading) {
		m_flatShader->disable();
	} else {
		m_normalShader->disable();
	}
}

void RenderPipeline::consume(CoordFrame* frame) {
	if (m_useFlatShading) {
		m_flatShader->setWVP(frame->getPCW());
		m_flatShader->setW(frame->getW());
	} else {
		m_normalShader->setWVP(frame->getPCW());
		m_normalShader->setW(frame->getW());
	}
}

CoordFrame* RenderPipeline::getCoordFrame() {
	return m_coordFrame;
}

void RenderPipeline::setFlatShading(bool val) {
	m_useFlatShading = val;
}