/*
 Copyright (C) 2006 So Yamaoka
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 Modified: Rex West (2015)
 		   Adam Dorwart (2016)
 
 */

#pragma once

#include <list>
#include <GL/glew.h>
#include <string>

#define INVALID_UNIFORM_LOCATION 0xffffffff
#define GLCheckError() (glGetError() == GL_NO_ERROR)

#ifndef SHADER_PATH
#define SHADER_PATH std::string("../src/shaders/")
#endif


/*! Handles GLSL shaders.  It can load the code from a file or read straight
 * from a char array. */
class Shader
{
public:
	Shader();
	~Shader();

	char* readFile(const char *filename);

	virtual bool init();

	void enable();

	bool addShader(GLenum ShaderType, const char* pFilename);

	bool finalize();

	GLint getUniformLocation(const char* pUniformName);

	GLint getProgramParam(GLint param);

private:
	GLuint m_shaderProg;
	typedef std::list<GLuint> ShaderObjList;
	ShaderObjList m_shaderObjList;
};
