#include "MaterialShader.h"

MaterialShader::MaterialShader() {

}

MaterialShader::~MaterialShader() {

}

bool MaterialShader::init() {
	return Shader::init();
}

void MaterialShader::setupMaterialLocations() {
	m_AmbientMapLocation = getUniformLocation("AmbientMap");
	m_DiffuseMapLocation = getUniformLocation("DiffuseMap");
	m_SpecularMapLocation = getUniformLocation("SpecularMap");
	m_SpecularExpLocation = getUniformLocation("shadowExpC");
	m_BumpMapLocation = getUniformLocation("BumpMap");
	m_ReflectionMapLocation = getUniformLocation("ReflectionMap");
}

void MaterialShader::bindMaterial(Material* material) {
	glUniform1i(m_AmbientMapLocation, AMBIENT_UNIT);
	glActiveTexture(GL_TEXTURE0 + AMBIENT_UNIT);
	material->bindAmbient();

	glUniform1i(m_DiffuseMapLocation, DIFFUSE_UNIT);
	glActiveTexture(GL_TEXTURE0 + DIFFUSE_UNIT);
	material->bindDiffuse();

	glUniform1i(m_SpecularMapLocation, SPECULAR_UNIT);
	glActiveTexture(GL_TEXTURE0 + SPECULAR_UNIT);
	material->bindSpecular();

	glUniform1i(m_BumpMapLocation, BUMP_UNIT);
	glActiveTexture(GL_TEXTURE0 + BUMP_UNIT);
	material->bindBump();

	glUniform1i(m_ReflectionMapLocation, REFLECTION_UNIT);
	glActiveTexture(GL_TEXTURE0 + REFLECTION_UNIT);
	material->bindReflection();

	glUniform1f(m_SpecularExpLocation, material->m_specularExp);
}

void MaterialShader::unbindMaterial(Material* material) {
	glActiveTexture(GL_TEXTURE0 + AMBIENT_UNIT);
	material->m_ambientMap.unbind();

	glActiveTexture(GL_TEXTURE0 + DIFFUSE_UNIT);
	material->m_diffuseMap.unbind();

	glActiveTexture(GL_TEXTURE0 + SPECULAR_UNIT);
	material->m_specularMap.unbind();

	glActiveTexture(GL_TEXTURE0 + BUMP_UNIT);
	material->m_bumpMap.unbind();
}