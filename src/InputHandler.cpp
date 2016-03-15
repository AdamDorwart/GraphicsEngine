#include "InputHandler.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

InputHandler::InputHandler(int _width, int _height, float _initialFov) {
	renderMeshA = true;
	leftMousePressed = false;
	flatShading = true;
	selectedObject = NULL;
	toggleMesh = true;
	lastMX = lastMY = 0;
	width = _width;
	height = _height;
	selectedMesh = NULL;
	lastTime = glfwGetTime();
	deltaTime = 0;
	initialFoV = _initialFov;
	FoV = _initialFov;
	position = vec3(0,0,0);
	direction = vec3(0,0,1);
	up = vec3(0,1,0);
	right = vec3(1,0,0);
	
	speed = 1.0;
	// horizontal angle : toward -Z
	horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	verticalAngle = 0.0f;
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
			case GLFW_KEY_4:
				// enable/disable flat shading
				flatShading = !flatShading;
				break;
			case GLFW_KEY_E:
				renderMeshA = !renderMeshA;
				break;
			case GLFW_KEY_R:
				*selectedObject = mat4(1.0);
				break;
			case GLFW_KEY_T:
				toggleMesh = !toggleMesh;
				break;
			case GLFW_KEY_W:
				if (toggleMesh) {
					position += direction*deltaTime*speed;
				} else if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(0,speed/4.0*deltaTime,0));
				}
				break;
			case GLFW_KEY_S:
				if (toggleMesh) {
					position += -direction*deltaTime*speed;
				} else if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(0,-deltaTime*speed/4.0,0));
				}
				break;
			case GLFW_KEY_A:
				if (toggleMesh) {
					position += -right*deltaTime*speed;
				} else if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(-deltaTime*speed/4.0,0,0));
				}
				break;
			case GLFW_KEY_D:
				if (toggleMesh) {
					position += right*deltaTime*speed;
				} else if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(deltaTime*speed/4.0,0,0));
				}
				break;
		}
	}
	if (action == GLFW_REPEAT) {
		switch (key) {
			case GLFW_KEY_W:
				if (toggleMesh) {
					position += direction*deltaTime*speed;
				} else if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(0,speed*deltaTime,0));
				}
				break;
			case GLFW_KEY_S:
				if (toggleMesh) {
					position += -direction*deltaTime*speed;
				} else if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(0,-deltaTime*speed,0));
				}
				break;
			case GLFW_KEY_A:
				if (toggleMesh) {
					position += -right*deltaTime*speed;
				} else if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(-deltaTime*speed,0,0));
				}
				break;
			case GLFW_KEY_D:
				if (toggleMesh) {
					position += right*deltaTime*speed;
				} else if (selectedObject != NULL) {
					*selectedObject = translate(*selectedObject, vec3(deltaTime*speed,0,0));
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
	if (toggleMesh) {
		float mouseSpeed = 0.05f;

		window->setMousePos(width/2, height/2);

		horizontalAngle += mouseSpeed * deltaTime * float(width/2 - xpos );
		verticalAngle   += mouseSpeed * deltaTime * float(height/2 - ypos );

		direction = vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		 // Right vector
		right = vec3(
		    sin(horizontalAngle - 3.14f/2.0f),
		    0,
		    cos(horizontalAngle - 3.14f/2.0f)
		);

		up = cross(right, direction);
	} else {
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
}

void InputHandler::consumeMouseScroll(Window* window, double xoffset, double yoffset) {
	FoV = initialFoV - .005*yoffset;
}
