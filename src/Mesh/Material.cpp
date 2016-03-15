#include "Material.h"

Material::Material() {
	
}

Material::~Material() {
	
}

void Material::setDiffuse(vec3 diffuseColor) {
	m_diffuseColor = diffuseColor;
}

void Material::setAmbient(vec3 ambientColor) {
	m_ambientColor = ambientColor;
}
void Material::setSpecular(vec3 specularColor) {
	m_specularColor = specularColor;
}
void Material::setSpecularExp(float exponent) {
	m_specularExp = exponent;
}


void Material::loadAmbient(const char* filename) {
	m_ambientMap.parseFile(filename);
}
void Material::loadDiffuse(const char* filename) {
	m_diffuseMap.parseFile(filename);
}
void Material::loadSpecular(const char* filename) {
	m_specularMap.parseFile(filename);
}
void Material::loadBump(const char* filename) {
	m_bumpMap.parseFile(filename);
}
void Material::loadReflection(const char* filename) {
	m_reflectionMap.parseFile(filename);
}