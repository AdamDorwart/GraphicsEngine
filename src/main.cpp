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
#include "RenderPipeline.h"
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
	ExpectsMsg(argc == 2, "Error: No mesh file provided.\n");

	int width = 2560;
	int height = 1440;

	// Setup Window
	Window window = Window(width, height, "Mesh Simplification");
	window.toggleFPS();
	handleGLerror();

	InputHandler* inputHandler = new InputHandler(width, height);
	inputHandler->subscribe(&window);

	// Setup Render Pipeline
	RenderPipeline pipeline;
	pipeline.init();
	//pipeline.addDirLight(vec3(3,3,0));

	SceneGraph* scene = new SceneGraph();

	// Setup mesh
	Mesh* meshA = new Mesh();
	ExpectsMsg(meshA->parseFile(argv[1]),
			   "Error: Unable to continue without mesh.\n");
	inputHandler->selectedMesh = meshA;
	//mat4* meshRef = meshA->getRefFrame();
	//*meshRef = rotate(*meshRef, -1.56f, vec3(1,0,0));

	SceneNode* transform = new SceneNode();

	transform->addChild(meshA);

	scene->addChild(transform);
	mat4* refFrame = inputHandler->selectedObject = transform->getRefFrame();

	// Setup view
	CoordFrame* frame = pipeline.getCoordFrame();

	// 1.0472 rad = 60 deg FOV
	double fov = 1.0472;
	double nearPlane = 0.1;
	double farPlane = 1000.0;
	double distanceY = meshA->getMaxHeight() / (2.0*tan(0.5*fov));
	double distanceX = meshA->getMaxWidth() / (2.0*tan(0.5*fov));
	double distance = (distanceY < distanceX) ? distanceY : distanceX;
	frame->setPerspective(fov, width, height, nearPlane, farPlane);
	/*
	vec3 d = vec3(2, -1, 0);
	vec3 c = vec3(2, 0, 1);
	vec3 up = vec3(0, 1, 0);
	*/
	vec3 d = vec3(0, 0, 5);
	vec3 c = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);
	frame->setCamera(d, c, up);

	// Run until user closes the window
	while (window.isActive()) {

  		// Render next frame
    	vec2 dim = window.initFrame();

    	inputHandler->width = dim.x;
    	inputHandler->height = dim.y;

    	pipeline.render(dim, scene);

        window.finalizeFrame();
        // Handle errors from rendering
        handleGLerror();

    }

    window.destroy();
    exit(EXIT_SUCCESS);
    return 0;
}