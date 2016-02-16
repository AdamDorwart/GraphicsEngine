#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Window.h"
#include "Shader.h"
#include "Mesh.h"

int main(int argc, char *argv[]) {
	assert(Logger::start());

	Window window = Window(2560, 1440);
	window.toggleFPS();

	Mesh mesh = Mesh();
	mesh.parseOFF("plane.off");
	mesh.edgeCollapse(13,12);
	
	Shader shader = Shader();
	shader.init();
	shader.addShader(GL_VERTEX_SHADER, "default.vs");
	shader.addShader(GL_FRAGMENT_SHADER, "default.fs");
	shader.finalize();

	// Run until user closes the window
	while (window.isActive())
    {
    	window.initFrame();
        
  		shader.enable();

  		mesh.render();

        window.finalizeFrame();
    }

    window.destroy();
    exit(EXIT_SUCCESS);
    return 0;
}
