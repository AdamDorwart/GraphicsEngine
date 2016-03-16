#include "Texture.h"
#include <vector>
#include <string>
#include "Contract.h"
#include "Util.h"
#include "Logger.h"
#include <cstring>

using namespace Util;

Texture::Texture() {
    id = 0;
    texType = GL_TEXTURE_2D;
}

Texture::~Texture() {
	if (id != 0) {
		glDeleteTextures(1, &id);
	}
}

void Texture::bind(void) {
    glBindTexture(texType, id);
}

void Texture::bindFB(GLenum attachment, GLenum point) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, point, id, 0);
}

void Texture::unbind(void) {
    glBindTexture(texType, 0);
}

bool Texture::parseFile(const char* filename) {
	std::vector<std::string> tokens;
	split(std::string(filename), '.', tokens);

	//Create ID for texture
    glGenTextures(1, &id);
	if (tokens.at(1).compare("ppm") == 0) {
		return ParsePPMTexture(this, filename);
	} else if (tokens.at(1).compare("tga") == 0) {
		return ParseTGATexture(this, filename);
	} else {
		LogError("Unable to load %s as texture: Unsupported file format\n", filename);
		return false;
	}
}

void Texture::setId(unsigned int _id) {
	id = _id;
}

void Texture::setTexType(unsigned int _type) {
	texType = _type;
}

void Texture::setDimensions(unsigned int _width, unsigned int _height) {
	width = _width;
	height = _height;
}

void Texture::setup(unsigned int _texType, unsigned int side) {
	EnsuresMsg(data.size() != 0,
			   "Unable to build Texture with no data.\n");

	texType = _texType;
	glBindTexture(texType, id);
    
    glTexImage2D(side, 0, colorType, width, height, 0, colorType, dataType, &data[0]);

    //Make sure no bytes are padded:
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    //And unbind it!
    glBindTexture(texType, 0);
}

