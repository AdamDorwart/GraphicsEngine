#include "FrameBuffer.h"
#include "Contract.h"

FrameBuffer::FrameBuffer() {
	m_fbo = 0;
	m_colorType = GL_NONE;
	m_depthType = GL_NONE;
}


FrameBuffer::~FrameBuffer() {
	if (m_fbo != 0) {
		glDeleteFramebuffers(1, &m_fbo);
	}
}

bool FrameBuffer::init(unsigned int width, unsigned int height, GLenum depthType, GLenum colorType) {
	m_colorType = colorType;
	m_depthType = depthType;
	m_width = width;
	m_height = height;

	GLenum colorF, colorT, depthF, depthT;

	switch (colorType) {
		case GL_RGBA32F:
			colorF = GL_RGBA;
			colorT = GL_FLOAT;
			break;
		case GL_R32F:
			colorF = GL_RED;
			colorT = GL_FLOAT;
			break;
		case GL_NONE:
			break;
	}

	switch (depthType) {
		case GL_DEPTH_COMPONENT16:
			depthF = GL_DEPTH_COMPONENT;
			depthT = GL_FLOAT;
			break;
		case GL_DEPTH_COMPONENT32F:
			depthF = GL_DEPTH_COMPONENT;
			depthT = GL_FLOAT;
			break;
		case GL_NONE:
			break;
	}

	// Create the FBO
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Create the textures
	if (colorType != GL_NONE) {
		GLuint colorTex;
		glGenTextures(1, &colorTex);

		glBindTexture(GL_TEXTURE_2D, colorTex);
		glTexImage2D(GL_TEXTURE_2D, 0, colorType, width, height, 0, colorF, colorT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };

		glDrawBuffers(1, DrawBuffers);

		m_colorTex.setId(colorTex);
		m_colorTex.setDimensions(width, height);
	}

	// Create the depth buffer 
	if (depthType != GL_NONE) {
		GLuint depthTex;
		glGenTextures(1, &depthTex);
		glBindTexture(GL_TEXTURE_2D, depthTex);

		// depth
		glTexImage2D(GL_TEXTURE_2D, 0, depthType, width, height, 0, depthF, depthT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

		if (colorType == GL_NONE) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		m_depthTex.setId(depthTex);
		m_depthTex.setDimensions(width, height);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	EnsuresMsg(status == GL_FRAMEBUFFER_COMPLETE,
		"FB error, status: 0x%x\n" 
		"\tIncomplete Attachment=0x%x\n"
		"\tIncomplete DrawBuffer=0x%x\n"
		"\tIncomplete ReadBuffer=0x%x\n"
		"\tMissing Attachment=0x%x\n"
		"\tUnsupported=0x%x\n"
		"\tUndefined=0x%x\n", status, 
		GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
		GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
		GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
		GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
		GL_FRAMEBUFFER_UNSUPPORTED,
		GL_FRAMEBUFFER_UNDEFINED);

	// restore default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void FrameBuffer::bindForWriting() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void FrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void FrameBuffer::bindForReading() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
}

void FrameBuffer::bindForReading(GLenum textureUnit) {
	glActiveTexture(textureUnit);

	if (m_colorType == GL_NONE) {
		m_depthTex.bind();
	} else {
		m_colorTex.bind();
	}
}

Texture* FrameBuffer::getDepthTexture() {
	return &m_depthTex;
}

Texture* FrameBuffer::getColorTexture() {
	return &m_colorTex;
}

void FrameBuffer::draw(unsigned int width, unsigned int height) {
	// Doesn't seem to work for depth FBOs

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	bindForReading();
	GLenum mask;
	if (m_depthType != GL_NONE) {
		mask = GL_DEPTH_BUFFER_BIT;
	} else if (m_colorType != GL_NONE) {
		mask = GL_COLOR_BUFFER_BIT;
	}
	// Needs to be GL_NEAREST for depth buffer blit
	glBlitFramebuffer(0, 0, m_width, m_height,
		0, 0, width, height, mask, GL_LINEAR);
}


unsigned int FrameBuffer::getWidth() {
	return m_width;
}

unsigned int FrameBuffer::getHeight() {
	return m_height;
}
