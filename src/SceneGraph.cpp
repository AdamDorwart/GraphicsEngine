#include "SceneGraph.h"

SceneNode::SceneNode() {
	m_refFrame = mat4(1.0f);
	m_visible = true;
}

SceneNode::~SceneNode() {
	
}

void SceneNode::traverse(CoordFrame* frame) {
	if (m_visible) {
		frame->pushWorldMatrix(m_refFrame);
		draw();
		for (auto node : m_children) {
			node->traverse(frame);
		}
		frame->popWorldMatrix();
	}
}

void SceneNode::draw() {
	// Children classes will implement this.
	// Must be defined so traverse works for generic SceneNodes
}

SceneNode* SceneNode::addChild(SceneNode* child) {
	m_children.push_back(child);
	return child;
}

SceneNode* SceneNode::removeChild(SceneNode* child) {
	m_children.remove(child);
	return child;
}

mat4* SceneNode::getRefFrame() {
	return &m_refFrame;
}

void SceneNode::setVisible(bool newVis) {
	m_visible = newVis;
}

bool SceneNode::getVisible() {
	return m_visible;
}