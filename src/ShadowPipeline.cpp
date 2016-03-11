#include "ShadowPipeline.h"
#include "Logger.h"

ShadowPipeline::ShadowPipeline() {
	m_shadowShader = new ShadowShader();
	m_normalShader = new NormalShader();
	m_cameraFrame = new CoordFrame();
	m_cameraFrame->subscribe(this);
}

ShadowPipeline::~ShadowPipeline() {
	delete m_shadowShader;
	delete m_normalShader;
	m_cameraFrame->unsubscribe(this);
	delete m_cameraFrame;
}

bool ShadowPipeline::init() {
	m_shadowShader->init();
	m_normalShader->init();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	return true;
}

void ShadowPipeline::render(vec2 dim, SceneGraph* scene) {
	// Render Shadow Maps
	m_shadowShader->enable();
	for (auto& light : m_dirLights) {

		light.fbo.bindForWriting();
		glViewport(0, 0, light.fbo.getWidth(), light.fbo.getHeight());

		glClear(GL_DEPTH_BUFFER_BIT);
		
		light.frame.subscribe(this);
		light.frame.resetWorldMatrix();

		//scene->traverse(&(light.frame));

		light.frame.unsubscribe(this);

		light.fbo.unbind();
	}
	m_shadowShader->disable();
	//m_dirLights[0].fbo.draw(dim.x, dim.y);
/*
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_cameraFrame->setViewport(0, 0, dim.x, dim.y);
	glViewport(0, 0, dim.x, dim.y);

	m_normalShader->enable();

	// Bind shadow maps for reading
	for (int i = 0; i < m_dirLights.size(); i++) {
		m_dirLights[i].fbo.bindForReading(GL_TEXTURE0 + i);
	}

	m_cameraFrame->resetWorldMatrix();

	scene->traverse(m_cameraFrame);

	m_normalShader->disable();*/
}

void ShadowPipeline::consume(CoordFrame* frame) {
	if (frame == m_cameraFrame) {
		m_normalShader->setWVP(frame->getPCW());
		m_normalShader->setW(frame->getW());
	} else {
		m_shadowShader->setWVP(frame->getPCW());
	}
}

CoordFrame* ShadowPipeline::getCameraFrame() {
	return m_cameraFrame;
}

void ShadowPipeline::addDirLight(vec3 direction) {
	const unsigned int shadow_w = 1024;
	const unsigned int shadow_h = 1024;

	Light light;
	light.direction = direction;
	//Encompass entire scene *hacky*
	light.frame.setViewport(0, 0, shadow_w, shadow_h);
	light.frame.setOrtho(-100,100,-100,100,-100,100);
	vec3 c = vec3(0,0,0);
	vec3 up = vec3(0,1,0);
	vec3 e = -direction;
	light.frame.setCamera(e, c, up);
	//light.fbo.init(shadow_w, shadow_h, GL_DEPTH_COMPONENT, GL_NONE);
	m_dirLights.push_back(light);
}
