#pragma once

#include "dengine/platform/CommonGL.h"
#include "dengine/util/BoundingBox.h"
#include "dengine/camera/Frustum.h"
#include "dengine/framebuffer/Framebuffer.h"

namespace Dg
{
class ShadowMap
{
  public:
	glm::mat4 m_lightView;
	glm::mat4 m_lightProjection;

	float m_zNear;
	float m_zFar;

	glm::vec3 m_origin;
	glm::vec3 m_target;
	float m_width;

  private:
	glm::mat4 m_croppedMatrix;

  public:
	WPtr<Framebuffer> m_shadowFBO;

	/**
	 * Calculates matrices that define the maximum bounds of the light space. That is the area in which this shadow map operates.
	 * No shadows will be rendered outside of this area. For a global sun lamp, this area should encompass the whole scene.
	 */
	inline void setupShadowVolume(const glm::vec3& origin, const glm::vec3& target, float width, float zNear, float zFar)
	{
		float hW = width / 2.0f;
		m_lightProjection = glm::ortho(-hW, hW, -hW, hW, zNear, zFar);
		m_lightView = glm::lookAt(origin, target, glm::vec3(0.0f, 1.0f, 0.0f));
		m_origin = origin;
		m_target = target;
		m_width = width;
		m_zNear = zNear;
		m_zFar = zFar;
	}

	inline glm::mat4 buildCropMatrix(const Frustum& frustum)
	{
		glm::mat4 lightPvm = m_lightProjection * m_lightView;
		BoundingBox box = frustum.createProjectedAABB(lightPvm);
		box.m_min.z = 0.0f; // Do not modify the near plane eg. "Extend" bounding box towards the original near plane
		return glm::ortho(box.m_min.x, box.m_max.x, box.m_min.y, box.m_max.y, box.m_min.z, box.m_max.z);
	}
};

} // namespace Dg
