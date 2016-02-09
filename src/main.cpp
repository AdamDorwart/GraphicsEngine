#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Window.h"
#include "Shader.h"

GLuint CreateVertexBuf() {
	float points[] = {
	   0.0f,  0.5f,  0.0f,
	   0.5f, -0.5f,  0.0f,
	  -0.5f, -0.5f,  0.0f
	};

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof (float), points, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	return vao;
}

int main(int argc, char *argv[]) {
	assert(Logger::start());

	Window window = Window(2560, 1440);
	window.toggleFPS();

	GLuint vao = CreateVertexBuf();
	
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

        glBindVertexArray (vao);
        glDrawArrays (GL_TRIANGLES, 0, 3);

        window.finalizeFrame();
    }

    window.destroy();
    exit(EXIT_SUCCESS);
    return 0;
}
