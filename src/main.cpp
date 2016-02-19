#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Logger.h"
#include "Window.h"
#include "InputHandler.h"
#include "RenderPipeline.h"
#include "SimpleShader.h"
#include "Mesh.h"
#include "CoordFrame.h"

using namespace glm;

void handleGLerror() {
	GLenum err = GL_NO_ERROR;
	bool error = false;
	while((err = glGetError()) != GL_NO_ERROR) {
		switch (err) {
			case GL_INVALID_ENUM:
				Logger::err("GL error: Invalid enum \n");
				break;
			case GL_INVALID_VALUE:
				Logger::err("GL error: Invalid value\n");
				break;
			case GL_INVALID_OPERATION:
				Logger::err("GL error: Invalid operation\n");
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				Logger::err("GL error: Invalid framebuffer operation\n");
				break;
			case GL_OUT_OF_MEMORY:
				Logger::err("GL error: Out of memory\n");
				break;
			default:
				Logger::err("GL error: Unknown");
		}
		error = true;
	}
	if (error) {
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char *argv[]) {
	assert(Logger::start());

	int width = 2560;
	int height = 1440;

	Window window = Window(width, height, "Mesh Simplification");
	window.toggleFPS();
	handleGLerror();

	InputHandler* inputHandler = new InputHandler();
	window.subscribe(Window::INPUT_EVENT, inputHandler);

	RenderPipeline pipeline;

	pipeline.init();
	CoordFrame* frame = pipeline.getCoordFrame();

	// 1.0472 rad = 60 deg FOV
	frame->setPerspective(1.0472, width, height, 1.0, 1000.0);
	vec3 c = vec3(0, 0, -5.0);
	vec3 d = vec3(0, 0, 1);
	vec3 up = vec3(0, 1, 0);
	frame->setCamera(c, d, up);

	SceneGraph* scene = new SceneGraph();

	Mesh* meshA = new Mesh();
	meshA->parseOFF("testpatch.off");
	meshA->pushEdgeCollapse(0,1);

	Mesh* meshB = new Mesh();
	//meshB.edgeCollapse(11,23);
	meshB->parseOFF("testpatch.off");
	meshB->pushEdgeCollapse(0,1);
	meshB->popEdgeCollapse();

	scene->addChild(meshA);
	scene->addChild(meshB);

	// Run until user closes the window
	while (window.isActive()) {

		if (inputHandler->renderMeshA) {
  			meshA->setVisible(true);
  			meshB->setVisible(false);

  			inputHandler->selectedObject = meshA->getRefFrame();
  		} else {
  			meshA->setVisible(false);
  			meshB->setVisible(true);

  			inputHandler->selectedObject = meshB->getRefFrame();
  		}

  		// Render next frame
    	vec2 dim = window.initFrame();

    	pipeline.render(dim, scene);

        window.finalizeFrame();
        // Handle errors from rendering
        handleGLerror();

    }

    window.destroy();
    exit(EXIT_SUCCESS);
    return 0;
}