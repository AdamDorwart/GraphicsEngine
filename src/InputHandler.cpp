#include "InputHandler.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

InputHandler::InputHandler(int _width, int _height) {
	renderMeshA = true;
	leftMousePressed = false;
	selectedObject = NULL;
	lastMX = lastMY = 0;
	width = _width;
	height = _height;
}

InputHandler::~InputHandler() {

}

void InputHandler::subscribe(Window* window) {
	window->subscribe(Window::KEY_INPUT_EVENT, this);
	window->subscribe(Window::MOUSE_BTN_EVENT, this);
	window->subscribe(Window::MOUSE_POS_EVENT, this);
	window->subscribe(Window::MOUSE_SCROLL_EVENT, this);
}

void InputHandler::unsubscribe(Window* window) {
	window->unsubscribe(Window::KEY_INPUT_EVENT, this);
	window->unsubscribe(Window::MOUSE_BTN_EVENT, this);
	window->unsubscribe(Window::MOUSE_POS_EVENT, this);
	window->unsubscribe(Window::MOUSE_SCROLL_EVENT, this);
}

void InputHandler::consumeKey(Window* window, int key, int scancode, int action, int mods) {
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
			case GLFW_KEY_R:
				*selectedObject = mat4(1.0);
				break;
			case GLFW_KEY_UP:
				if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(0, 0.5, 0));
				}
				break;
			case GLFW_KEY_DOWN:
				if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(0, -0.5, 0));
				}
				break;
			case GLFW_KEY_LEFT:
				if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(0.5, 0, 0));
				}
				break;
			case GLFW_KEY_RIGHT:
				if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(-0.5, 0, 0));
				}
				break;	
		}
	}
}

void InputHandler::consumeMouseBtn(Window* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		leftMousePressed = true; 
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		leftMousePressed = false;
	}
}

void InputHandler::consumeMousePos(Window* window, double xpos, double ypos) {
	vec3 v, w, axis;
	float dv, dw, angle;
	if (leftMousePressed) {
		v = vec3((2.0*xpos - (float)(width)) / (float)(width), ((float)(height)-2.0*ypos) / (float)(height), 0);
		dv = length(v);
		dv = (dv < 1.0) ? dv : 1.0;
		v[2] = sqrtf(1.001 - dv*dv);
		v = normalize(v);

		w = vec3((2.0*lastMX - (float)(width)) / (float)(width), ((float)(height)-2.0*lastMY) / (float)(height), 0);
		dw = length(w);
		dw = (dw < 1.0) ? dw : 1.0;
		w[2] = sqrtf(1.001 - dw*dw);
		w = normalize(w);

		axis = cross(v, w);
		axis = normalize(axis);
		angle = acos(dot(v, w) / (length(v)*length(w)));

		if (axis[0] != 0 && axis[1] != 0 && axis[2] != 0 && std::isfinite(angle)) {
			*selectedObject = rotate(*selectedObject, angle, axis);
		}
	}
	lastMX = xpos;
	lastMY = ypos;
}

void InputHandler::consumeMouseScroll(Window* window, double xoffset, double yoffset) {
	double scale = 0.5;
	*selectedObject = translate(*selectedObject, vec3(0, 0, yoffset*scale));
}
