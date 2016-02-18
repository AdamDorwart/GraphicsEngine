#pragma once
#include "Window.h"

class InputHandler : public InputListener {
	public:
		bool renderMeshA;
		
		InputHandler();
		~InputHandler();
		virtual void consume(Window* window, int key, int scancode, int action, int mods);
};