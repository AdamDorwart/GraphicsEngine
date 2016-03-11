#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Logger.h"
#include "Contract.h"
#include "Window.h"
#include "InputHandler.h"
#include "ShadowPipeline.h"
#include "LightShader.h"
#include "Mesh.h"
#include "CoordFrame.h"

using namespace glm;

void handleGLerror() {
	GLenum err = GL_NO_ERROR;
	bool error = false;
	while((err = glGetError()) != GL_NO_ERROR) {
		switch (err) {
			case GL_INVALID_ENUM:
				LogError("GL error: Invalid enum \n");
				break;
			case GL_INVALID_VALUE:
				LogError("GL error: Invalid value\n");
				break;
			case GL_INVALID_OPERATION:
				LogError("GL error: Invalid operation\n");
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				LogError("GL error: Invalid framebuffer operation\n");
				break;
			case GL_OUT_OF_MEMORY:
				LogError("GL error: Out of memory\n");
				break;
			default:
				LogError("GL error: Unknown");
		}
		error = true;
	}
	if (error) {
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char *argv[]) {
	Expects(Logger::start());

	int width = 2560;
	int height = 1440;

	int shadowBufSize = 4096;

	// 1.0472 rad = 60 deg FOV
	double fov = 1.0472;
	double nearPlane = 0.1;
	double farPlane = 1000.0;

	// Setup Window
	Window window = Window(width, height, "Depth Test");
	window.toggleFPS();
	window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	handleGLerror();

	InputHandler* inputHandler = new InputHandler(width, height, fov);
	inputHandler->subscribe(&window);

	SceneGraph* scene = new SceneGraph();
	CoordFrame* cameraFrame = new CoordFrame();
	CoordFrame* lightFrame = new CoordFrame();

	// Setup mesh
	/*
	Mesh* meshA = new Mesh();
	ExpectsMsg(meshA->parseFile("hall.off"),
			   "Error: Unable to continue without mesh.\n");
	mat4* meshRef = meshA->getRefFrame();
	*meshRef = rotate(*meshRef, -1.56f, vec3(1,0,0));
	inputHandler->selectedObject = meshA->getRefFrame();

	scene->addChild(meshA);

	vec3 cameraPos = vec3(0, 5, -5);
	vec3 cameraLookAt = vec3(0, 0, 0);
	vec3 cameraUp = vec3(0, 1, 0);
	vec3 lightPos = vec3(0,-5, -3);
	vec3 lightLookAt = vec3(0, 10, 5);
	vec3 lightUp = vec3(0, 1, 0);
	lightFrame->setOrtho(-100,100,-100,100,-100,100);

	*/

	Mesh* meshA = new Mesh();
	ExpectsMsg(meshA->parseFile("teapot.off"),
			   "Error: Unable to continue without mesh.\n");
	inputHandler->selectedObject = meshA->getRefFrame();

	Mesh* meshB = new Mesh();
	ExpectsMsg(meshB->parseFile("plane.off"),
			   "Error: Unable to continue without mesh.\n");
	mat4* meshRef = meshB->getRefFrame();
	*meshRef = scale(*meshRef, vec3(10.0,10.0,10.0));
	*meshRef = rotate(*meshRef, -1.56f, vec3(1,0,0));

	SceneNode* transform = new SceneNode();	

	transform->addChild(meshA);
	transform->addChild(meshB);
	scene->addChild(transform);

	vec3 cameraPos = vec3(0, 5, -5);
	vec3 cameraLookAt = vec3(0, 0, 0);
	vec3 cameraUp = vec3(0, 1, 0);
	vec3 lightPos = vec3(-1, 5, 0);
	vec3 lightLookAt = vec3(0, 0, 0);
	vec3 lightUp = vec3(0, 1, 0);
	lightFrame->setOrtho(-10,10,-10,10,-10,10);


	inputHandler->position = cameraPos;
	inputHandler->up = cameraUp;
	
	cameraFrame->setPerspective(fov, width, height, nearPlane, farPlane);
	lightFrame->setViewport(0, 0, shadowBufSize, shadowBufSize);

	cameraFrame->setCamera(cameraPos, cameraLookAt, cameraUp);
	lightFrame->setCamera(lightPos, lightLookAt, lightUp);

	FrameBuffer fbo;
	fbo.init(shadowBufSize, shadowBufSize, GL_DEPTH_COMPONENT32F, GL_NONE);
	ShadowShader* shadow = new ShadowShader();
	LightShader* light = new LightShader();
	shadow->init();
	light->init();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	lightFrame->subscribe(shadow);
	cameraFrame->subscribe(light);
	
	// Run until user closes the window
	while (window.isActive()) {
		double currentTime = glfwGetTime();
		inputHandler->deltaTime = float(currentTime - inputHandler->lastTime);
		inputHandler->lastTime = currentTime;

  		// Render next frame
    	vec2 dim = window.initFrame();
    	glCullFace(GL_FRONT);
    	shadow->enable();

    	glViewport(0, 0, shadowBufSize, shadowBufSize);
    	fbo.bindForWriting();

    	glClear(GL_DEPTH_BUFFER_BIT);
    	lightFrame->resetWorldMatrix();

    	scene->traverse(lightFrame);

    	shadow->disable();

    	glBindFramebuffer(GL_FRAMEBUFFER, 0);

		cameraFrame->setViewport(0, 0, dim.x, dim.y);
		cameraFrame->setPerspective(inputHandler->FoV, dim.x, dim.y, nearPlane, farPlane);
		cameraPos = inputHandler->position;
		cameraLookAt = cameraPos + inputHandler->direction;
		cameraUp = inputHandler->up;
		cameraFrame->setCamera(
			cameraPos,
			cameraLookAt,
			cameraUp);
		glViewport(0, 0, dim.x, dim.y);
		glCullFace(GL_BACK);
		light->enable();
		light->setDepthWVP(lightFrame->getPCW());
		light->setViewPos(cameraPos);
		light->setLightPos(lightPos);

		fbo.bindForReading(GL_TEXTURE0);

		cameraFrame->resetWorldMatrix();

		scene->traverse(cameraFrame);

		fbo.unbind();

		light->disable();

        window.finalizeFrame();
        // Handle errors from rendering
        handleGLerror();

    }

    window.destroy();
    exit(EXIT_SUCCESS);
    return 0;
}