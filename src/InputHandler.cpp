#include "InputHandler.h"
#include <GLFW/glfw3.h>

InputHandler::InputHandler() {
	renderMeshA = true;
}

InputHandler::~InputHandler() {

}

void InputHandler::consume(Window* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				window->destroy();
				exit(EXIT_SUCCESS);
				break;
			case GLFW_KEY_1:
				static bool fillState = true;
				fillState = !fillState;
				if (fillState) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				} else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				break;
			case GLFW_KEY_2:
				static bool cwState = true;
				cwState = !cwState;
				if (cwState) {
					glFrontFace(GL_CW);
				} else {
					glFrontFace(GL_CCW);
				}
				break;
			case GLFW_KEY_3:
				// Changes flat shading 
				static bool firstProvokeState = false;
				firstProvokeState = !firstProvokeState;
				if (firstProvokeState) {
					glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
				} else {
					glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
				}
				break;
			case GLFW_KEY_E:
				renderMeshA = !renderMeshA;
				break;
		}
	}
}