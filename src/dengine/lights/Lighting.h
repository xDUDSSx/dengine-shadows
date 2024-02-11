#pragma once

#include <memory>
#include <vector>

#include "dengine/platform/Common.h"

#include "ShadowSunLight.h"

namespace Dg
{
class Light;
class PhongShader;
class ShadowSunLight;

/**
 * \brief Class that manages passing of light information to shaders.
 */
class Lighting
{
	std::vector<Ptr<Light>> lights;

public:
  	ShadowSunLight m_shadowSunLight; // TODO: (DR) Turn into a list later

	/**
	 * \brief Set light information uniforms for a shader.
	 * \param shader The shader
	 */
	void setUniforms(const PhongShader& shader) const;

	void addLight(Light* light);
	void addLight(const std::shared_ptr<Light>& light);

	void removeLight(std::shared_ptr<Light>& light);

	std::vector<std::shared_ptr<Light>> getLights() const;
};

} // namespace Dg
