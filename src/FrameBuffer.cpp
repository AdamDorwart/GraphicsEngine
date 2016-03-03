#include "FrameBuffer.h"
#include "Logger.h"

FrameBuffer::FrameBuffer() {
	m_fbo = 0;
	m_texture = 0;
	m_depth = 0;
	m_internalType = GL_NONE;
}


FrameBuffer::~FrameBuffer() {
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
	}

	if (m_texture != 0) {
		glDeleteTextures(1, &m_texture);
	}

	if (m_depth != 0) {
		glDeleteTextures(1, &m_depth);
	}
}

bool FrameBuffer::init(unsigned int width, unsigned int height, bool depth, GLenum internalType) {
	m_internalType = type;
	m_width = width;
	m_height = height;

	GLenum format, type;

	switch (internalType) {
		case GL_RGB32F:
			format = GL_RGB;
			type = GL_FLOAT;
			break;
		case GL_R32F:
			format = GL_RED;
			type = GL_FLOAT;
			break;
		case GL_NONE:
			break;
	}

	// Create the FBO
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Create the textures
	if (internalType != GL_NONE) {
		glGenTextures(1, &m_texture);

		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, format, type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };

		glDrawBuffers(1, DrawBuffers);
	}

	// Create the depth buffer 
	if (depth) {
		glGenTextures(1, &m_depth);

		// depth
		glBindTexture(GL_TEXTURE_2D, m_depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LogErr("FB error, status: 0x%x\n", status);
		return false;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

void FrameBuffer::bindForWriting() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}


void FrameBuffer::bindForReading() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
}
void FrameBuffer::bindForReading(GLenum textureUnit) {
	glActiveTexture(textureUnit);

	if (m_internalType == GL_NONE) {
		glBindTexture(GL_TEXTURE_2D, m_depth);
	} else {
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}
}


unsigned int FrameBuffer::getWidth() {
	return width;
}

unsigned int FrameBuffer::getHeight() {
	return height;
}
