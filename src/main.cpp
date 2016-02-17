#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Window.h"
#include "SimpleShader.h"
#include "Mesh.h"
#include "CoordFrame.h"


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

	// Changes flat shading
	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

	CoordFrame* frame = new CoordFrame();
	window.setCoordFrame(frame);

	// 60 deg FOV
	frame->setPerspective(1.0472, width, height, 1.0, 1000.0);
	vec3 c = vec3(0, 0, -2.0);
	vec3 d = vec3(0, 0, 1);
	vec3 up = vec3(0, 1, 0);
	frame->setCamera(c, d, up);

	Mesh mesh = Mesh();
	mesh.parseOFF("plane.off");
	//mesh.edgeCollapse(13,12);
	
	SimpleShader shader = SimpleShader(frame);
	shader.init();

	// Run until user closes the window
	while (window.isActive()) {
    	window.initFrame();

  		shader.enable();

  		frame->resetWorldMatrix();

  		mesh.render(frame);

        window.finalizeFrame();

       	handleGLerror();
    }

    window.destroy();
    exit(EXIT_SUCCESS);
    return 0;
}