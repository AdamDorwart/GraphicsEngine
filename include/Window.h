#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <list>
#include "Logger.h"
#include "CoordFrame.h"

using namespace glm;

class InputListener;

class Window {
	public:
		enum EventType {INPUT_EVENT};

		Window();
		Window(int width, int height, const char* title);
		~Window();

		void initialize(int width, int height, const char* title);

		void destroy();

		bool isActive();

		vec2 initFrame();

		void finalizeFrame();

		bool toggleFPS();

		void subscribe(EventType event, InputListener* listener);
		void unsubscribe(EventType event, InputListener* listener);

	private:
		void updateFPS();

		void handleKey(int key, int scancode, int action, int mods);

		double m_lastTime;
		int m_frames;
		bool m_showFPS;
		GLFWwindow* m_glfwWindow;

		// List of subscribed listeners
		std::list<InputListener*> m_inputListeners;

	// Statics
	private:
		static void initGLFW();

		static void errorCallback(int error, const char* description);

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static std::unordered_map<GLFWwindow*, Window*> activeWindows;
		static bool isGlfwInit;
};

class InputListener {
	public:
		virtual void consume(Window* window, int key, int scancode, int action, int mods) = 0;
};