#include "RenderPipeline.h"

RenderPipeline::RenderPipeline() {
	m_shader = new SimpleShader();
	m_coordFrame = new CoordFrame();
	m_coordFrame->subscribe(this);
}

RenderPipeline::~RenderPipeline() {
	delete m_shader;
	delete m_coordFrame;
	m_coordFrame->unsubscribe(this);
}

bool RenderPipeline::init() {
	m_shader->init();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	return true;
}

void RenderPipeline::render(vec2 dim, SceneGraph* scene) {
	m_coordFrame->setViewport(0, 0, dim.x, dim.y);

	m_shader->enable();

	m_coordFrame->resetWorldMatrix();

	scene->traverse(m_coordFrame);

	m_shader->disable();
}

void RenderPipeline::consume(CoordFrame* frame) {
	m_shader->setWVP(frame->getPCW());
	m_shader->setW(frame->getW());
}

CoordFrame* RenderPipeline::getCoordFrame() {
	return m_coordFrame;
}