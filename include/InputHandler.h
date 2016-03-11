#pragma once
#include <glm/glm.hpp>
#include "Window.h"
#include "Mesh.h"

class InputHandler : public InputListener {
	public:
		int width, height;
		bool renderMeshA;
		bool flatShading;
		bool toggleMesh;

		double lastTime;
		float deltaTime;

		vec3 direction;
		vec3 right;
		vec3 position;
		vec3 up;
		float speed;
		// horizontal angle : toward -Z
		float horizontalAngle;
		// vertical angle : 0, look at the horizon
		float verticalAngle;
		float FoV;
		float initialFoV;
	
		mat4* selectedObject;

		Mesh* selectedMesh;

		bool leftMousePressed;
		int lastMX, lastMY;
		
		InputHandler(int width, int height, float initialFoV);
		~InputHandler();

		void subscribe(Window* window);
		void unsubscribe(Window* window);

		virtual void consumeKey(Window* window, int key, int scancode, int action, int mods);
		virtual void consumeMouseBtn(Window* window, int button, int action, int mods);
		virtual void consumeMousePos(Window* window, double xpos, double ypos);
		virtual void consumeMouseScroll(Window* window, double xoffset, double yoffset);
};