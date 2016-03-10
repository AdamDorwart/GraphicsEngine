#include "FrameBuffer.h"
#include "Contract.h"

FrameBuffer::FrameBuffer() {
	m_fbo = 0;
	m_texture = 0;
	m_depthTex = 0;
	m_colorType = GL_NONE;
}


FrameBuffer::~FrameBuffer() {
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
	}

	if (m_texture != 0) {
		glDeleteTextures(1, &m_texture);
	}

	if (m_depthTex != 0) {
		glDeleteTextures(1, &m_depthTex);
	}
}

bool FrameBuffer::init(unsigned int width, unsigned int height, GLenum depthType, GLenum colorType) {
	m_colorType = colorType;
	m_depthType = depthType;
	m_width = width;
	m_height = height;

	GLenum format, type;

	switch (colorType) {
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
	if (colorType != GL_NONE) {
		glGenTextures(1, &m_texture);

		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, format, type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };

		glDrawBuffers(1, DrawBuffers);
	}

	// Create the depth buffer 
	if (depthType != GL_NONE) {
		glGenTextures(1, &m_depthTex);
		glBindTexture(GL_TEXTURE_2D, m_depthTex);

		// depth
		glTexImage2D(GL_TEXTURE_2D, 0, depthType, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTex, 0);
		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTex, 0);

		if (colorType == GL_NONE) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	EnsuresMsg(status == GL_FRAMEBUFFER_COMPLETE,
		"FB error, status: 0x%x\n", status);

	// restore default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (colorType == GL_NONE) {
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}

	return true;
}

bool FrameBuffer::initDepth() {
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	GLuint depthTexture;
	glGenTextures(1, &m_depthTex);
	glBindTexture(GL_TEXTURE_2D, m_depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTex, 0);

	glDrawBuffer(GL_NONE); // No color buffer is drawn to.

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	// Always check that our framebuffer is ok
	EnsuresMsg(status == GL_FRAMEBUFFER_COMPLETE,
		"FB error, status: 0x%x\n", status);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void FrameBuffer::bindForWriting() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	if (m_colorType == GL_NONE) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
}

void FrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (m_colorType == GL_NONE) {
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}
}


void FrameBuffer::bindForReading() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
}
void FrameBuffer::bindForReading(GLenum textureUnit) {
	glActiveTexture(textureUnit);

	if (m_colorType == GL_NONE) {
		glBindTexture(GL_TEXTURE_2D, m_depthTex);
	} else {
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}
}

void FrameBuffer::draw(unsigned int width, unsigned int height) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	bindForReading();
	GLenum mask;
	if (m_depthType != GL_NONE) {
		mask = GL_DEPTH_BUFFER_BIT;
	} else if (m_colorType != GL_NONE) {
		mask = GL_COLOR_BUFFER_BIT;
	}
	// Needs to be GL_NEAREST for depth buffer blit
	glBlitFramebuffer(0, 0, width, height,
		0, 0, width, height, mask, GL_LINEAR);
}


unsigned int FrameBuffer::getWidth() {
	return m_width;
}

unsigned int FrameBuffer::getHeight() {
	return m_height;
}
