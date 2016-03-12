#pragma once
#include <GLFW/glfw3.h>


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

		void* data;
		unsigned int size;

	public:
		Texture();
		~Texture();

		void bind();
		void unbind();

		bool parseFile(const char* filename);

		void setId(unsigned int id);
		void setDimensions(unsigned int width, unsigned int height);

};
