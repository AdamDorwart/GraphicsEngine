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

#include <cstdio>
#include <cstdlib>

#include "Shader.h"
#include "Logger.h"


Shader::Shader()
{
	m_shaderProg = 0;
}

Shader::~Shader()
{
	// Delete the intermediate shader objects that have been added to the program
	// The list will only contain something if shaders were compiled but the object itself
	// was destroyed prior to linking.
	for (ShaderObjList::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++)
	{
		glDeleteShader(*it);
	}

	if (m_shaderProg != 0)
	{
		glDeleteProgram(m_shaderProg);
		m_shaderProg = 0;
	}
}

char* Shader::readFile(const char *filename)
{
	char* shaderFile = 0;
	
    //Open the file
	FILE* fp = fopen((std::string(SHADER_PATH) + filename).c_str() , "rb");
	if(!fp) {
		Logger::err("File doesn't exist [%s]", (std::string(SHADER_PATH) + filename).c_str());
        exit(EXIT_FAILURE);
    }
    
	//Obtain the file size
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);
    
	//Alloc memory - will be deleted while setting the shader up
	shaderFile = new char[size+1];
    
	//Copy the file to the shaderFile
	fread(shaderFile, sizeof(char), size, fp);
	shaderFile[size]='\0'; //Eliminate the garbage at EOF
	fclose(fp);
    
	return shaderFile;
}

bool Shader::init()
{
	m_shaderProg = glCreateProgram();

	if (m_shaderProg == 0) {
		Logger::err("Error creating shader program\n");
		return false;
	}

	return true;
}

// Use this method to add shaders to the program. When finished - call finalize()
bool Shader::addShader(GLenum ShaderType, const char* pFilename)
{
	std::string s;

	s = readFile(pFilename);

	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		Logger::err("Error creating shader type %d\n", ShaderType);
		return false;
	}

	// Save the shader object - will be deleted in the destructor
	m_shaderObjList.push_back(ShaderObj);

	const GLchar* p[1];
	p[0] = s.c_str();
	GLint Lengths[1] = { (GLint)s.size() };

	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj);

	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLchar InfoLog[2048];
		glGetShaderInfoLog(ShaderObj, sizeof(InfoLog), NULL, InfoLog);
		Logger::err("Error compiling '%s': '%s'\n", pFilename, InfoLog);
		return false;
	}

	glAttachShader(m_shaderProg, ShaderObj);

	return true;
}


// After all the shaders have been added to the program call this function
// to link and validate the program.
bool Shader::finalize()
{
	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(m_shaderProg);

	glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
		Logger::err("Error linking shader program: '%s'\n", ErrorLog);
		return false;
	}

	glValidateProgram(m_shaderProg);
	glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
	}

	// Delete the intermediate shader objects that have been added to the program
	for (auto shader : m_shaderObjList) {
		glDeleteShader(shader);
	}

	m_shaderObjList.clear();

	return GLCheckError();
}


void Shader::enable() {
	glUseProgram(m_shaderProg);
}

void Shader::disable() {
	glUseProgram(0);
}


GLint Shader::getUniformLocation(const char* pUniformName)
{
	GLint Location = glGetUniformLocation(m_shaderProg, pUniformName);

	if (Location == -1) {
		Logger::err("Warning! Unable to get the location of uniform '%s'\n", pUniformName);
	}

	return Location;
}

GLint Shader::getProgramParam(GLint param)
{
	GLint ret;
	glGetProgramiv(m_shaderProg, param, &ret);
	return ret;
}
