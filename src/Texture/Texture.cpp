#include "Texture.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Util.h"
#include "Logger.h"

using namespace Util;

Texture::Texture() {
    id = 0;
}

Texture::~Texture() {
	if (data != NULL) {
		delete[] data;
	}
	if (id != 0) {
		glDeleteTextures(1, &id);
	}
}

void Texture::bind(void) {
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind(void) {
    glBindTexture(GL_TEXTURE_2D, 0);
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

void Texture::setDimensions(unsigned int _width, unsigned int _height) {
	width = _width;
	height = _height;
}


