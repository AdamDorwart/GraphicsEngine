#pragma once

class Texture;
bool ParsePPMTexture(Texture* tex, const char* filename);
bool ParseTGATexture(Texture* tex, const char* filename);
class Texture {
	friend bool ParsePPMTexture(Texture* tex, const char* filename);
	friend bool ParseTGATexture(Texture* tex, const char* filename);
	protected:
		unsigned int id;
		const char* filename;
		unsigned int width, height;

		void* data;
		unsigned int size;

	public:
		Texture();
		~Texture();

		void bind();
		void unbind();

		bool parseFile(const char* filename);

};
