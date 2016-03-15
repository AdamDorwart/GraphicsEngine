#pragma once
#include "Shader.h"
#include "Material.h"

class MaterialShader : public Shader {
	public:
		enum {AMBIENT_UNIT = 0, DIFFUSE_UNIT, SPECULAR_UNIT, BUMP_UNIT, REFLECTION_UNIT, NEXT_AVIL_UNIT};

		MaterialShader();
		virtual ~MaterialShader();

		virtual bool init();

		void setupMaterialLocations();

		void bindMaterial(Material* material);

		void unbindMaterial(Material* material);
	private:

		GLint m_AmbientMapLocation;
		GLint m_DiffuseMapLocation;
		GLint m_SpecularMapLocation;
		GLint m_SpecularExpLocation;
		GLint m_BumpMapLocation;
		GLint m_ReflectionMapLocation;
};