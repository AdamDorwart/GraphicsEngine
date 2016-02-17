#include "CoordFrame.h"
#include <glm/gtc/matrix_transform.hpp>

CoordFrame::CoordFrame() {
	// Add identity matrix
	w.push_back(mat4(1.0f));
}


CoordFrame::~CoordFrame() {

}

mat4 CoordFrame::getP() {
	return p;
}

mat4 CoordFrame::getPCW() {
	return p * c * getW();
}

mat4 CoordFrame::getCW() {
	return c * getW();
}

mat4 CoordFrame::getW() {
	mat4 world = mat4(1.0f);
	for (auto mat : w) {
		world = world * mat;
	}
	return world;
}

void CoordFrame::setViewport(float xmin, float xmax, float ymin, float ymax) {
	d = vec4(xmin, ymin, xmax, ymax);
	if (xmax != 0 && ymax != 0) {
		setPerspective(fov, xmax, ymax, nearFace, farFace);
	}
}

void CoordFrame::setPerspective(float _fov, float _width, float _height, float _nearFace, float _farFace) {
	fov = _fov;
	nearFace = _nearFace;
	farFace = _farFace;
	p = perspectiveFov(_fov, _width, _height, _nearFace, _farFace);
	broadcast();
}

void CoordFrame::setCamera(vec3& e, vec3& d, vec3& up) {
	c = lookAt(e, d, up);
	broadcast();
}

mat4 CoordFrame::getCamera() {
	return c;
}

vec4 CoordFrame::getViewport() {
	return d;
}

mat4 CoordFrame::getPerspective() {
	return p;
}

void CoordFrame::pushWorldMatrix(mat4& mat) {
	w.push_back(mat);
	broadcast();
}

void CoordFrame::popWorldMatrix() {
	w.pop_back();
	broadcast();
}

void CoordFrame::resetWorldMatrix() {
	w.clear();
	w.push_back(mat4(1.0f));
	broadcast();
}

void CoordFrame::subscribe(CoordFrameListener* listener) {
	m_listeners.push_back(listener);
}

void CoordFrame::unsubscribe(CoordFrameListener* listener) {
	m_listeners.remove(listener);
}

void CoordFrame::broadcast() {
	for (auto listener : m_listeners) {
		listener->consume(this);
	}
}