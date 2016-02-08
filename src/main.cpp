#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


static void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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
	// Init GLFW
	if (!glfwInit()) {
    	exit(EXIT_FAILURE);	
    }
    // Set error callback for GLFW
    glfwSetErrorCallback(ErrorCallback);

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
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(glewErr));
	  exit(EXIT_FAILURE);
	}

	// Set swap interval to 1 to prevent wasted frames
	glfwSwapInterval(1);

	// Set input callback
	glfwSetKeyCallback(window, KeyCallback);

	// Set Framebuffer resize callback
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	GLuint vao = CreateVertexBuf();

	// Run until user closes the window
	while (!glfwWindowShouldClose(window))
    {
    	float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);

        // wipe the drawing surface clear
  		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //glBindVertexArray (vao);
        //glDrawArrays (GL_TRIANGLES, 0, 3);
        
        glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
        glEnd();
        
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
