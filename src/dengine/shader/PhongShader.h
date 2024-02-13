
#pragma once

#include "ObjectShader.h"

#include "dengine/resources/Mesh.h"

namespace Dg
{
/**
 * Shader for lit objects with textures.
 */
class PhongShader : public ObjectShader
{
public:
	enum LightingModel
	{
		PHONG,
		BLINN_PHONG
	};

protected:
	// Material uniforms
	GLint material_diffuse;
	GLint material_specular;
	GLint material_ambient;
	GLint material_shininess;

	// Texture uniforms
	GLint normalStrength;

	GLint tintId;

	GLint m_lightingModelId;

	// Shadow mapping
	GLint m_shadowMapLoc; // TODO: Add support for multiple shadow samplers

	GLint m_visualizeShadowMapLoc;
	GLint m_shadowBiasLoc;

	// TODO: (DR) Location IDs should probably be stored in some kind of map with string keys ideally

public:
	glm::vec3 m_tint{1.0f};
	LightingModel m_lightingModel{BLINN_PHONG};
	GLuint m_shadowMapId;

	bool m_isUsingPSSM = true;
	bool m_visualizeShadowMap = true;
	float m_shadowBias = 0.001f;

	explicit PhongShader(GLuint id);

	void init(bool initSuperclass) override;

	void setUniforms() override;
	void setUniformsPerMeshPart(Dg::Mesh::MeshPart& meshPart) override;

protected:
	void setMaterialUniforms(Dg::Mesh::Material material);
	void bindTextures(Dg::Mesh::TextureSet tSet);

	/**
	 * \brief Sets all texture active flags to false. Used to reset texture state
	 * before binding new set of textures.
	 */
	void clearTextures() const;
	void bindTexture(GLuint textureID, const std::string& type, int typeIndex, GLuint textureUnit);
};
} // namespace Dg