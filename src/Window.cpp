#include "Window.h"

#include <stdio.h>
#include <stdlib.h>

Window::Window() {
	m_lastTime = glfwGetTime();
	m_frames = 0;
	m_showFPS = false;
}

Window::Window(int width, int height) {
	m_lastTime = glfwGetTime();
	m_frames = 0;
	m_showFPS = false;
	initialize(width, height);
}

Window::~Window() {
	destroy();
}

void Window::destroy() {
	glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}

void Window::updateFPS() {
	double curTime = glfwGetTime();
	double dt = curTime - m_lastTime;
	if (dt > 0.5) {
		m_lastTime = curTime;
		double fps = (double)m_frames / dt;
		char tmp[128];
		sprintf (tmp, "fps: %.2f", fps);
		glfwSetWindowTitle (m_glfwWindow, tmp);
		m_frames = 0;
	}
	m_frames++;
}

void Window::initialize(int width, int height) {
	// start GL context and O/S window using the GLFW helper library
	Logger::info("starting GLFW %s\n", glfwGetVersionString());

	// Set error callback for GLFW
    glfwSetErrorCallback(Window::errorCallback);

    // Magic for drivers that don't expose features
    glewExperimental = GL_TRUE;

	// Init GLFW
	if (!glfwInit()) {
		Logger::err("ERROR: could not start GLFW3\n");
    	exit(EXIT_FAILURE);	
    }

    // Set OpenGL version
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    m_glfwWindow = glfwCreateWindow(width, height, "Mesh Simplification", NULL, NULL);
    if (!m_glfwWindow) {
    	Logger::err("ERROR: could not create window\n");
	    glfwTerminate();
	    exit(EXIT_FAILURE);
	}
	// Set the OpenGL context
	glfwMakeContextCurrent(m_glfwWindow);

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
	glfwSetKeyCallback(m_glfwWindow, Window::keyCallback);

	// Set Framebuffer resize callback
	glfwSetFramebufferSizeCallback(m_glfwWindow, Window::framebufferSizeCallback);
}

bool Window::isActive() {
	return !glfwWindowShouldClose(m_glfwWindow);
}

bool Window::toggleFPS() {
	return m_showFPS = !m_showFPS;
}

void Window::initFrame() {
	if (m_showFPS) {
		updateFPS();
	}

	// wipe the drawing surface clear
  	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::finalizeFrame() {
	// Swap front and back buffers
	glfwSwapBuffers(m_glfwWindow);

	// Poll for and process events
	glfwPollEvents();
}

void Window::errorCallback(int error, const char* description) {
	Logger::err("GLFW ERROR: code %i msg: %s\n", error, description);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}