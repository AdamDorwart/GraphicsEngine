#pragma once
#include "Texture.h"

class CubeMap {
	private:
		unsigned int id;
		
		Texture m_front;
		Texture m_back;
		Texture m_top;
		Texture m_bottom;
		Texture m_left;
		Texture m_right;

	public:
		CubeMap();
		~CubeMap();

		void bind();
		void unbind();

		bool load(const char* filename);
};