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

	// Parameters of the light projection volume
	glm::vec3 m_origin;
	glm::vec3 m_target;
	glm::vec3 m_dir;
	float m_width;

	std::vector<Ptr<GameObject>> m_receivers;
	std::vector<Ptr<GameObject>> m_casters;
	Frustum m_cameraFrustum;
	BoundingBox m_cameraFrustumAABB;
	Frustum m_tightCameraFrustum;
	BoundingBox m_tightCameraFrustumAABB;

	glm::mat4 m_cropMatrix{1.0f};
	glm::mat4 m_croppedLightProjection{1.0f};

	BoundingBox m_testBox;
//	std::vector<glm::vec3> m_testBox2;

  public:
	WPtr<Framebuffer> m_shadowFBO;

	/**
	 * Calculates matrices that define the maximum bounds of the light space. That is the area in which this shadow map operates.
	 * No shadows will be rendered outside of this area. For a global sun lamp, this area should encompass the whole scene.
	 */
	void setupShadowVolume(const glm::vec3& origin, const glm::vec3& target, float width, float zNear, float zFar);

	void update(Scene& scene, AbstractCamera& camera);

  private:
	//	void buildCropMatrix(const Frustum& frustum);
	void buildCropMatrix(const BoundingBox& box);

	void computeTightShadowFrustum(AbstractCamera& camera, Scene& scene);

	void precalculateBoundingBoxes(Scene& scene);

	static std::vector<Ptr<GameObject>> findShadowReceivers(const BoundingBox& box, Scene& scene);
	std::vector<Ptr<GameObject>> findShadowCasters(BoundingBox frustumAABB, Scene& scene);

	static std::pair<float, float> findTightNearAndFarPlanes(const glm::vec3& origin, const glm::vec3& dir,
	                                                         const std::vector<Ptr<GameObject>> objects);
};

} // namespace Dg
