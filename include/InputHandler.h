#pragma once
#include <glm/glm.hpp>
#include "Window.h"
#include "Mesh.h"

class InputHandler : public InputListener {
	public:
		int width, height;
		bool renderMeshA;
	
		mat4* selectedObject;

		Mesh* selectedMesh;

		bool leftMousePressed;
		int lastMX, lastMY;
		
		InputHandler(int width, int height);
		~InputHandler();

		void subscribe(Window* window);
		void unsubscribe(Window* window);

		virtual void consumeKey(Window* window, int key, int scancode, int action, int mods);
		virtual void consumeMouseBtn(Window* window, int button, int action, int mods);
		virtual void consumeMousePos(Window* window, double xpos, double ypos);
		virtual void consumeMouseScroll(Window* window, double xoffset, double yoffset);
};