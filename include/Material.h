#pragma once
#include <glm/glm.hpp>
#include "Texture.h"

#ifndef MATERIAL_PATH
#define MATERIAL_PATH "../media/material/"
#endif

using namespace glm;

class MaterialShader;
class Material {
	friend class MaterialShader;
	private:
		Texture m_diffuseMap;
		Texture m_specularMap;
		Texture m_ambientMap;
		Texture m_bumpMap;
		Texture m_reflectionMap;

		vec3 m_diffuseColor;
		vec3 m_specularColor;
		float m_specularExp;
		vec3 m_ambientColor;

	public:
		Material();
		~Material();

		void setDiffuse(vec3 diffuseColor);
		void setAmbient(vec3 ambientColor);
		void setSpecular(vec3 specularColor);
		void setSpecularExp(float exponent);

		void loadAmbient(const char* filename);
		void loadDiffuse(const char* filename);
		void loadSpecular(const char* filename);
		void loadBump(const char* filename);
		void loadReflection(const char* filename);
};