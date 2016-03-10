#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <list>

using namespace glm;

class CoordFrameListener;

class CoordFrame {
	private:
		// Viewport
		vec4 d;
		// Perspective
		mat4 p;
		float fov;
		float nearFace;
		float farFace;
		// Camera
		mat4 c;
		// World stack 
		// (a vector works just as well here and gives us some better properties)
		std::vector<mat4> w;

		// List of subscribed listeners
		std::list<CoordFrameListener*> m_listeners;
		void broadcast();

	public:
		CoordFrame();
		~CoordFrame();

		mat4 getP();
		mat4 getPCW();
		mat4 getCW();
		mat4 getW();

		void setViewport(float xmin, float xmax, float ymin, float ymax);
		void setPerspective(float fov, float width, float height, float nearFace, float farFace);
		void setOrtho(float left, float right, float bottom, float top, float nearFace, float farFace);
		void setCamera(vec3& e, vec3& center, vec3& up);
		mat4 getCamera();
		vec4 getViewport();
		mat4 getPerspective();

		void pushWorldMatrix(mat4& mat);
		void popWorldMatrix();
		void resetWorldMatrix();

		void subscribe(CoordFrameListener* listener);
		void unsubscribe(CoordFrameListener* listener);
};

class CoordFrameListener {
	public:
		virtual void consume(CoordFrame* frame) = 0;
};