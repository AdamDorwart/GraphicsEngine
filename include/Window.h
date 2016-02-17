#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "Logger.h"
#include "CoordFrame.h"

class Window {
	public:
		Window();
		Window(int width, int height, const char* title);
		~Window();

		void initialize(int width, int height, const char* title);

		void destroy();

		bool isActive();

		void initFrame();

		void finalizeFrame();

		bool toggleFPS();

		CoordFrame* getCoordFrame();
		CoordFrame* setCoordFrame(CoordFrame* frame);

	private:
		void updateFPS();

		void handleKey(int key, int scancode, int action, int mods);

		double m_lastTime;
		int m_frames;
		bool m_showFPS;
		GLFWwindow* m_glfwWindow;
		CoordFrame* m_coordFrame;

	// Statics
	private:
		static void initGLFW();

		static void errorCallback(int error, const char* description);

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static std::unordered_map<GLFWwindow*, Window*> activeWindows;
		static bool isGlfwInit;
};