#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#ifndef TEXTURE_PATH
#define TEXTURE_PATH "../media/"
#endif

class Texture;
bool ParsePPMTexture(Texture* tex, const char* filename, unsigned int texType=GL_TEXTURE_2D, unsigned int side=GL_TEXTURE_2D);
bool ParseTGATexture(Texture* tex, const char* filename, unsigned int texType=GL_TEXTURE_2D, unsigned int side=GL_TEXTURE_2D);
class Texture {
	friend bool ParsePPMTexture(Texture* tex, const char* filename, unsigned int texType, unsigned int side);
	friend bool ParseTGATexture(Texture* tex, const char* filename, unsigned int texType, unsigned int side);
	protected:
		unsigned int id;
		unsigned int width, height;
		unsigned int colorType;
		unsigned int dataType;
		unsigned int texType;

		std::vector<unsigned char> data;
		unsigned int size;

	public:
		Texture();
		~Texture();

		void bind();
		void bindFB(GLenum attachment, GLenum point);
		void unbind();

		bool parseFile(const char* filename);

		void setId(unsigned int id);
		void setDimensions(unsigned int width, unsigned int height);
		void setTexType(unsigned int texType);

		void setup(unsigned int textureType=GL_TEXTURE_2D, unsigned int side=GL_TEXTURE_2D);
};
