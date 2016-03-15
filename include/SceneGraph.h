#pragma once
#include <glm/glm.hpp>
#include "CoordFrame.h"

using namespace glm;

class SceneNode;
using SceneGraph = SceneNode;

class SceneNode {
	private:
		std::list<SceneNode*> m_children;
	protected:
		mat4 m_refFrame;
		bool m_visible;
		bool m_useDebugDraw;
	public:
		SceneNode();
		~SceneNode();

		void traverse(CoordFrame* frame);
		virtual void draw();
		virtual void drawDebug(CoordFrame* frame);

		SceneNode* addChild(SceneNode* child);
		SceneNode* removeChild(SceneNode* child);

		mat4* getRefFrame();

		void setVisible(bool newVis);
		bool getVisible();

		void setDebugDraw(bool newState);
		bool getDebugDraw();
};