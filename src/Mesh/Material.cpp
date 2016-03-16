#include "Material.h"

Material::Material() {
	m_hasAmbientMap = false;
	m_hasDiffuseMap = false;
	m_hasSpecularMap = false;
	m_hasBumpMap = false;
	m_hasReflectionMap = false;
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
	m_hasAmbientMap = m_ambientMap.parseFile(filename);
}
void Material::loadDiffuse(const char* filename) {
	m_hasDiffuseMap = m_diffuseMap.parseFile(filename);
}
void Material::loadSpecular(const char* filename) {
	m_hasSpecularMap = m_specularMap.parseFile(filename);
}
void Material::loadBump(const char* filename) {
	m_hasBumpMap = m_bumpMap.parseFile(filename);
}
void Material::loadReflection(const char* filename) {
	m_hasReflectionMap = m_reflectionMap.parseFile(filename);
}

void Material::bindAmbient() {
	if (m_hasAmbientMap) {
		m_ambientMap.bind();
	}
}

void Material::bindDiffuse() {
	if (m_hasDiffuseMap) {
		m_diffuseMap.bind();
	}
}

void Material::bindSpecular() {
	if (m_hasSpecularMap) {
		m_specularMap.bind();
	}
}

void Material::bindBump() {
	if (m_hasBumpMap) {
		m_bumpMap.bind();
	}
}

void Material::bindReflection() {
	if (m_hasReflectionMap) {
		m_reflectionMap.bind();
	}
}