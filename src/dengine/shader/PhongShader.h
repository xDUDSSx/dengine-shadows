
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
	GLint m_lightMatrixLoc;  // TODO: Remove
	GLint m_lightViewLoc;  // TODO: Remove

	// TODO: (DR) Location IDs should probably be stored in some kind of map with string keys ideally

public:
	glm::vec3 m_tint{1.0f};
	LightingModel m_lightingModel{BLINN_PHONG};
	GLuint m_shadowMapId;
//	glm::mat4 m_lightMatrix{1.0f}; // TODO: (DR) Move to a light UBO
	glm::vec3 m_lightPos{1.0f};
	glm::mat4 m_lightView{1.0f};

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