#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Texture.h"

class FrameBuffer
{
	public:

		FrameBuffer();

		~FrameBuffer();

		bool init(unsigned int width, unsigned int height, GLenum depthType, GLenum colorType);
		
		void unbind();
		void bindForWriting();

		void bindForReading();
		void bindForReading(GLenum textureUnit);

		Texture* getDepthTexture();
		Texture* getColorTexture();

		void draw(unsigned int width, unsigned int height);

		unsigned int getWidth();
		unsigned int getHeight();

	private:
		unsigned int m_width, m_height;
		GLuint m_fbo;
		Texture m_colorTex;
		Texture m_depthTex;
		GLenum m_depthType;
		GLenum m_colorType;
};


