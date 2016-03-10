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

	// Setup Window
	Window window = Window(width, height, "Depth Test");
	window.toggleFPS();
	handleGLerror();

	InputHandler* inputHandler = new InputHandler(width, height);
	inputHandler->subscribe(&window);

	SceneGraph* scene = new SceneGraph();
	CoordFrame* cameraFrame = new CoordFrame();
	CoordFrame* lightFrame = new CoordFrame();
	CoordFrame* quadFrame = new CoordFrame();

	// Setup mesh
	Mesh* meshA = new Mesh();
	ExpectsMsg(meshA->parseFile("hall.off"),
			   "Error: Unable to continue without mesh.\n");
	mat4* meshRef = meshA->getRefFrame();
	*meshRef = rotate(*meshRef, -1.56f, vec3(1,0,0));
	inputHandler->selectedObject = meshA->getRefFrame();

	scene->addChild(meshA);

	Mesh* quad = new Mesh();
	ExpectsMsg(quad->parseFile("quad.obj"),
			   "Error: Unable to continue without mesh.\n");

	// 1.0472 rad = 60 deg FOV
	double fov = 1.0472;
	double nearPlane = 0.1;
	double farPlane = 1000.0;
	cameraFrame->setPerspective(fov, width, height, nearPlane, farPlane);
	lightFrame->setViewport(0, 0, 1024, 1024);
	quadFrame->setPerspective(fov, width, height, nearPlane, farPlane);
	//lightFrame->setOrtho(-100,100,-100,100,-100,100);
	//lightFrame->setPerspective(fov, 1024, 1024, nearPlane, farPlane);
	/*
	vec3 d = vec3(2, -1, 0);
	vec3 c = vec3(2, 0, 1);
	vec3 up = vec3(0, 1, 0);
	*/
	vec3 d = vec3(0, 5, -5);
	vec3 c = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);
	cameraFrame->setCamera(d, c, up);

	vec3 direction = vec3(-1,-2,3);
	lightFrame->setCamera(direction, c, up);

	vec3 quadDir = vec3(0, 0,-2);
	quadFrame->setCamera(quadDir, c , up);

	FrameBuffer fbo;
	fbo.initDepth();
	ShadowShader* shadow = new ShadowShader();
	LightShader* light = new LightShader();
	shadow->init();
	light->init();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	lightFrame->subscribe(shadow);
	quadFrame->subscribe(light);
	//cameraFrame->subscribe(light);
	
	// Run until user closes the window
	while (window.isActive()) {

  		// Render next frame
    	vec2 dim = window.initFrame();
    	
    	shadow->enable();

    	fbo.bindForWriting();
    	glViewport(0, 0, 1024, 1024);

    	glClear(GL_DEPTH_BUFFER_BIT);
    	lightFrame->resetWorldMatrix();

    	scene->traverse(lightFrame);

    	shadow->disable();

    	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    	fbo.bindForReading();

		//cameraFrame->setViewport(0, 0, dim.x, dim.y);
		quadFrame->setViewport(0, 0, dim.x, dim.y);
		glViewport(0, 0, dim.x, dim.y);

		light->enable();
		light->setDepthWVP(lightFrame->getPCW());

		fbo.bindForReading(GL_TEXTURE0);

		//cameraFrame->resetWorldMatrix();
		quadFrame->resetWorldMatrix();

		//scene->traverse(cameraFrame);
		((SceneGraph*)quad)->traverse(quadFrame);

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