#pragma once
#include <GLFW/glfw3.h>

class FrameBuffer
{
	public:

		FrameBuffer();

		~FrameBuffer();

		bool init(unsigned int width, unsigned int height, bool depth, GLenum internalType);

		void bindForWriting();

		void bindForReading();
		void bindForReading(GLenum textureUnit);

		unsigned int getWidth();
		unsigned int getHeight();

	private:
		unsigned int m_width, m_height;
		GLuint m_fbo;
		GLuint m_texture;
		GLuint m_depth;
		GLenum m_internalType;
};


