#pragma once
#include <glm/glm.hpp>
#include "Window.h"

class InputHandler : public InputListener {
	public:
		bool renderMeshA;
		mat4* selectedObject;
		
		InputHandler();
		~InputHandler();
		virtual void consume(Window* window, int key, int scancode, int action, int mods);
};