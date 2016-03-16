#include "CubeMap.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Util.h"
#include "Logger.h"

using namespace Util;

CubeMap::CubeMap() {
	id = 0;
}

CubeMap::~CubeMap() {
	
}

/*
void CubeMap::createFBCubeMap(unsigned int width, unsigned int height, GLenum depthType, GLenum colorType) {
	m_fboFront = new Framebuffer();
	m_fboFront->init(width, height, depthType, colorType);
	if (depthType != GL_NONE) {
		m_front = m_fboFront->getDepthTexture();
	} else {
		m_front = m_fboFront->getColorTexture();
	}

	m_fboBack = new Framebuffer();
	m_fboBack->init(width, height, depthType, colorType);
	if (depthType != GL_NONE) {
		m_back = m_fboBack->getDepthTexture();
	} else {
		m_back = m_fboBack->getColorTexture();
	}

	m_fboTop = new Framebuffer();
	m_fboTop->init(width, height, depthType, colorType);
	if (depthType != GL_NONE) {
		m_top = m_fboTop->getDepthTexture();
	} else {
		m_top = m_fboTop->getColorTexture();
	}

	m_fboBottom = new Framebuffer();
	m_fboBottom->init(width, height, depthType, colorType);
	if (depthType != GL_NONE) {
		m_bottom = m_fboBottom->getDepthTexture();
	} else {
		m_bottom = m_fboBottom->getColorTexture();
	}

	m_fboLeft = new Framebuffer();
	m_fboLeft->init(width, height, depthType, colorType);
	if (depthType != GL_NONE) {
		m_left = m_fboLeft->getDepthTexture();
	} else {
		m_left = m_fboLeft->getColorTexture();
	}

	m_fboRight = new Framebuffer();
	m_fboRight->init(width, height, depthType, colorType);
	if (depthType != GL_NONE) {
		m_right = m_fboRight->getDepthTexture();
	} else {
		m_right = m_fboRight->getColorTexture();
	}
	
}*/

void CubeMap::bind(void) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void CubeMap::unbind(void) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

bool CubeMap::load(const char* filename) {
	std::vector<std::string> tokens;
	split(std::string(filename), '.', tokens);
	
	//Create ID for texture
	glGenTextures(1, &id);
	m_front.setId(id);
	m_back.setId(id);
	m_top.setId(id);
	m_bottom.setId(id);
	m_left.setId(id);
	m_right.setId(id);

	bool (*Parser)(Texture*, const char*, GLenum, GLenum);
	if (tokens.at(1).compare("ppm") == 0) {
		Parser = &ParsePPMTexture;
	} else if (tokens.at(1).compare("tga") == 0) {
		Parser = &ParseTGATexture;
	} else {
		LogError("Unable to load %s as cube map: Unsupported file format\n", filename);
		return false;
	}
	bool ret = true;
	ret = ret && Parser(&m_front, 
						(tokens.at(0) + "_front." + tokens.at(1)).c_str(), 
						GL_TEXTURE_CUBE_MAP,
						GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	ret = ret && Parser(&m_back, 
						(tokens.at(0) + "_back." + tokens.at(1)).c_str(), 
						GL_TEXTURE_CUBE_MAP,
						GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	ret = ret && Parser(&m_top, 
						(tokens.at(0) + "_top." + tokens.at(1)).c_str(), 
						GL_TEXTURE_CUBE_MAP,
						GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	ret = ret && Parser(&m_bottom, 
						(tokens.at(0) + "_bottom." + tokens.at(1)).c_str(), 
						GL_TEXTURE_CUBE_MAP,
						GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	ret = ret && Parser(&m_left, 
						(tokens.at(0) + "_left." + tokens.at(1)).c_str(), 
						GL_TEXTURE_CUBE_MAP,
						GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	ret = ret && Parser(&m_right, 
						(tokens.at(0) + "_right." + tokens.at(1)).c_str(), 
						GL_TEXTURE_CUBE_MAP,
						GL_TEXTURE_CUBE_MAP_POSITIVE_X);

	bind();
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	unbind();

	return ret;
}