#pragma once

#include <set>

#include "dengine/platform/CommonGL.h"
#include "dengine/camera/Frustum.h"
#include "dengine/util/BoundingBox.h"
#include "dengine/data/RenderOptions.h"
#include "dengine/framebuffer/DepthAttachment.h"
#include "dengine/data/DisplayOptions.h"

namespace Dg
{
class Scene;
class GameObject;
class AbstractCamera;
class Framebuffer;

class ShadowMap
{
  public:
	RenderOptions::ShadowType m_shadowType;

	glm::mat4 m_lightView;
	glm::mat4 m_lightProjection;

	float m_zNear;
	float m_zFar;

	float m_zNearTight;
	float m_zFarTight;

	// Parameters of the light projection volume
	glm::vec3 m_origin;
	glm::vec3 m_target;
	glm::vec3 m_dir;
	float m_width;

	float m_splitSchemeWeight = 0.5f;

	std::vector<Ptr<GameObject>> m_receivers;
	std::set<GameObject*> m_casters;

	Frustum m_cameraFrustum;
	BoundingBox m_cameraFrustumAABB;
	Frustum m_tightCameraFrustum;
	BoundingBox m_tightCameraFrustumAABB;

	int m_splitCount{4};
	std::vector<Frustum> m_splitFrustums;
	std::vector<BoundingBox> m_splitFrustumsAABBs;
	std::vector<float> m_splitPositions;
	std::vector<glm::mat4> m_cropMatrices;
	std::vector<glm::mat4> m_lightPvmMatrices;

	// Debug
	std::vector<GameObject*> m_debugCasters;
	long long m_cpuUpdateTime{0};
	long long m_cpuRenderTime{0};

	/**
	 * Sets the position and direction of the light space volume.
	 * The scene should be ahead of the plane defined by the origin and direction vector.
	 */
	void setupShadowVolume(const glm::vec3& origin, const glm::vec3& target, float width, float zNear, float zFar);

	void update(RenderOptions::ShadowType shadowType, int cascadeCount, Scene& scene, AbstractCamera& camera);

	Ptr<Framebuffer> createShadowFramebuffer(RenderOptions::ShadowType shadowType, int cascades, int resolution);

	void drawShadowBuffer(WPtr<Framebuffer> shadowFBOPtr, const RenderOptions& renderOptions, const DisplayOptions& displayOptions);

  private:
	static glm::mat4 buildSceneInDependentCropMatrix(const BoundingBox& frustumAABB, const glm::mat4& lightPvm);
	static glm::mat4 buildSceneDependentCropMatrix(const BoundingBox& frustumAABB, const glm::mat4& lightPvm,
	                                               const std::vector<GameObject*>& casters,
	                                               const std::vector<Ptr<GameObject>>& receivers);

	void computeTightShadowFrustum(AbstractCamera& camera, Scene& scene);

	static std::vector<Ptr<GameObject>> findShadowReceivers(const BoundingBox& box, Scene& scene);
	std::vector<GameObject*> findShadowCasters(BoundingBox frustumAABB, int splitIndex, Scene& scene);

	static std::pair<float, float> findTightNearAndFarPlanes(const glm::vec3& origin, const glm::vec3& dir,
	                                                         const std::vector<Ptr<GameObject>>& objects);
};

} // namespace Dg
