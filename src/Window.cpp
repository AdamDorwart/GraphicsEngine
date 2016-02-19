#include "Window.h"

#include <stdio.h>
#include <stdlib.h>

std::unordered_map<GLFWwindow*, Window*> Window::activeWindows;
bool Window::isGlfwInit = false;

Window::Window() {
	m_lastTime = glfwGetTime();
	m_frames = 0;
	m_showFPS = false;
}

Window::Window(int width, int height, const char* title) {
	m_lastTime = glfwGetTime();
	m_frames = 0;
	m_showFPS = false;
	initialize(width, height, title);
}

Window::~Window() {
	destroy();
}

void Window::destroy() {
	activeWindows.erase(m_glfwWindow);
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

void Window::initGLFW() {
	if (!isGlfwInit) {
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
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		isGlfwInit = true;
	}
}

void Window::initialize(int width, int height, const char* title) {
	// Initialize GLFW
	initGLFW();

    // Create window
    m_glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!m_glfwWindow) {
    	Logger::err("ERROR: could not create window\n");
	    glfwTerminate();
	    exit(EXIT_FAILURE);
	}
	activeWindows[m_glfwWindow] = this;

	glfwMakeContextCurrent(m_glfwWindow);

	// Init GLEW for this OpenGL context
    GLenum glewErr = glewInit();
	if (GLEW_OK != glewErr) {
		/* Problem: glewInit failed, something is seriously wrong. */
		Logger::err("Error: %s\n", glewGetErrorString(glewErr));
		exit(EXIT_FAILURE);
	}
	//Consume the false Invalid enum error caused by GLEW
	//https://www.opengl.org/wiki/OpenGL_Loading_Library
	glGetError();

	// VSync 1 = enable, 0 = disable
	glfwSwapInterval(0);

	// Set input callback
	glfwSetKeyCallback(m_glfwWindow, Window::keyCallback);

}

bool Window::isActive() {
	return !glfwWindowShouldClose(m_glfwWindow);
}

bool Window::toggleFPS() {
	return m_showFPS = !m_showFPS;
}

vec2 Window::initFrame() {
	int width, height;
	// Make this windows OpenGL context active
	glfwMakeContextCurrent(m_glfwWindow);

	// Get framebuffer size and set the viewport
	glfwGetFramebufferSize(m_glfwWindow, &width, &height);
    glViewport(0, 0, width, height);

	if (m_showFPS) {
		updateFPS();
	}
	
	// wipe the drawing surface clear
  	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  	return vec2(width, height);
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
	for (auto listener : activeWindows[window]->m_inputListeners) {
		listener->consume(activeWindows[window], key, scancode, action, mods);
	}
}

void Window::subscribe(EventType event, InputListener* listener) {
	switch (event) {
		case INPUT_EVENT:
			m_inputListeners.push_back(listener);
			break;
	}
}

void Window::unsubscribe(EventType event, InputListener* listener) {
	switch (event) {
		case INPUT_EVENT:
			m_inputListeners.remove(listener);
			break;
	}
}