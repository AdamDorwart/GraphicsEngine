#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Logger.h"


static void ErrorCallback(int error, const char* description) {
    Logger::err("GLFW ERROR: code %i msg: %s\n", error, description);
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static GLuint CreateShader(const char* vertexShader, const char* fragmentShader) {
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShader, NULL);
	glCompileShader(fs);

	GLuint shader = glCreateProgram ();
	glAttachShader(shader, fs);
	glAttachShader(shader, vs);
	glLinkProgram(shader);

	return shader;
}

static GLuint CreateVertexBuf() {
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

	// start GL context and O/S window using the GLFW helper library
	Logger::info("starting GLFW %s\n", glfwGetVersionString());

	// Set error callback for GLFW
    glfwSetErrorCallback(ErrorCallback);

	// Init GLFW
	if (!glfwInit()) {
		Logger::err("ERROR: could not start GLFW3\n");
    	exit(EXIT_FAILURE);	
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(2560, 1440, "Mesh Simplification", NULL, NULL);
    if (!window) {
	    glfwTerminate();
	    exit(EXIT_FAILURE);
	}
	// Set the OpenGL context
	glfwMakeContextCurrent(window);

	// Init GLEW
    GLenum glewErr = glewInit();
	if (GLEW_OK != glewErr) {
		/* Problem: glewInit failed, something is seriously wrong. */
		Logger::err("Error: %s\n", glewGetErrorString(glewErr));
		exit(EXIT_FAILURE);
	}

	// Set swap interval to 1 to prevent wasted frames
	glfwSwapInterval(1);

	// Set input callback
	glfwSetKeyCallback(window, KeyCallback);

	// Set Framebuffer resize callback
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	GLuint vao = CreateVertexBuf();

	const char* vertexShader =
		"#version 400\n"
		"in vec3 vp;"
		"void main () {"
		"  gl_Position = vec4 (vp, 1.0);"
		"}";

	const char* fragmentShader =
		"#version 400\n"
		"out vec4 frag_colour;"
		"void main () {"
		"  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
		"}";
	GLuint shader = CreateShader(vertexShader, fragmentShader);

	// Run until user closes the window
	while (!glfwWindowShouldClose(window))
    {
    	float ratio;
        int width, height;
/*
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);*/

        // wipe the drawing surface clear
  		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  		glUseProgram(shader);

  		/*
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        */

        glBindVertexArray (vao);
        glDrawArrays (GL_TRIANGLES, 0, 3);

        /*
        glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
        glEnd();
        */

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
