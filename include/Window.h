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
		enum EventType {KEY_INPUT_EVENT, MOUSE_BTN_EVENT, MOUSE_POS_EVENT, MOUSE_SCROLL_EVENT};

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

		void setMousePos(double xpos, double ypos);
		void setInputMode(int mode, int value);
		
	private:
		void updateFPS();

		void handleKey(int key, int scancode, int action, int mods);

		double m_lastTime;
		int m_frames;
		bool m_showFPS;
		GLFWwindow* m_glfwWindow;

		// List of subscribed listeners
		std::list<InputListener*> m_keyInputListeners;
		std::list<InputListener*> m_mouseButtonListeners;
		std::list<InputListener*> m_mousePositionListeners;
		std::list<InputListener*> m_mouseScrollListeners;

	// Statics
	private:
		static void initGLFW();

		static void errorCallback(int error, const char* description);
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
		static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		static std::unordered_map<GLFWwindow*, Window*> activeWindows;
		static bool isGlfwInit;
};

class InputListener {
	public:
		virtual void consumeKey(Window* window, int key, int scancode, int action, int mods) = 0;
		virtual void consumeMouseBtn(Window* window, int button, int action, int mods) = 0;
		virtual void consumeMousePos(Window* window, double xpos, double ypos) = 0;
		virtual void consumeMouseScroll(Window* window, double xoffset, double yoffset) = 0;
};