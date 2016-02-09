#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Logger.h"

class Window {
	public:
		Window();
		Window(int width, int height);
		~Window();

		void initialize(int width, int height);

		void destroy();

		bool isActive();

		void initFrame();

		void finalizeFrame();

		bool toggleFPS();

	private:
		void updateFPS();

		static void errorCallback(int error, const char* description);

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

		double m_lastTime;
		int m_frames;
		bool m_showFPS;
		GLFWwindow* m_glfwWindow;
};