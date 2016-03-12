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
	if (tokens.at(1).compare("ppm") == 0) {
		return ParsePPMTexture(this, filename);
	} else if (tokens.at(1).compare("tga") == 0) {
		return ParseTGATexture(this, filename);
	} else {
		LogError("Unable to load %s as texture: Unsupported file format\n", filename);
		return false;
	}
}


