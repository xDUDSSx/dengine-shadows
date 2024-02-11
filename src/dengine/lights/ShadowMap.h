#pragma once

#include "dengine/platform/CommonGL.h"
#include "dengine/camera/Frustum.h"
#include "dengine/util/BoundingBox.h"

namespace Dg
{
class Scene;
class GameObject;
class AbstractCamera;
class Framebuffer;

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

	std::vector<Ptr<GameObject>> m_receivers;
	Frustum m_cameraFrustum;
	BoundingBox m_cameraFrustumAABB;
	Frustum m_tightCameraFrustum;

  private:
	glm::mat4 m_croppedMatrix;

  public:
	WPtr<Framebuffer> m_shadowFBO;

	/**
	 * Calculates matrices that define the maximum bounds of the light space. That is the area in which this shadow map operates.
	 * No shadows will be rendered outside of this area. For a global sun lamp, this area should encompass the whole scene.
	 */
	void setupShadowVolume(const glm::vec3& origin, const glm::vec3& target, float width, float zNear, float zFar);

	glm::mat4 buildCropMatrix(const Frustum& frustum);

	void computeTightShadowFrustum(AbstractCamera& camera, Scene& scene);

  private:
	static std::vector<Ptr<GameObject>> findShadowReceivers(const BoundingBox& box, Scene& scene);

	static std::pair<float, float> findTightNearAndFarPlanes(const glm::vec3& origin, const glm::vec3& dir,
	                                                                const std::vector<Ptr<GameObject>> objects);
};

} // namespace Dg
