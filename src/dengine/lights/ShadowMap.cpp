#include "ShadowMap.h"

#include "dengine/scene/Scene.h"
#include "dengine/framebuffer/Framebuffer.h"
#include "dengine/camera/AbstractCamera.h"
#include "dengine/util/Collisions.h"

namespace Dg
{

void ShadowMap::setupShadowVolume(const glm::vec3& origin, const glm::vec3& target, float width, float zNear, float zFar)
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

glm::mat4 ShadowMap::buildCropMatrix(const Frustum& frustum)
{
	glm::mat4 lightPvm = m_lightProjection * m_lightView;
	BoundingBox box = frustum.createProjectedAABB(lightPvm);
	box.m_min.z = 0.0f; // Do not modify the near plane eg. "Extend" bounding box towards the original near plane
	return glm::ortho(box.m_min.x, box.m_max.x, box.m_min.y, box.m_max.y, box.m_min.z, box.m_max.z);
}

void ShadowMap::computeTightShadowFrustum(AbstractCamera& camera, Scene& scene)
{
	m_cameraFrustum = GfxUtils::unprojectMatrix(camera.getProjection() * camera.getView());
	m_cameraFrustumAABB = m_cameraFrustum.createAABB();
	m_receivers = findShadowReceivers(m_cameraFrustumAABB, scene);
//	LOG_INFO("Receiver count: {}", m_receivers.size());
	if (!m_receivers.empty())
	{
		auto tightNearFar = findTightNearAndFarPlanes(camera.getPosition(), camera.getDirection(), m_receivers);
		// Ensure that the tight near and far planes stay within original range, and that far is greater than near
		float tightNear = std::max(tightNearFar.first, camera.getZNear());
		float tightFar = std::max(tightNearFar.second, tightNear + 0.01f);
		glm::mat4 tightProjection = AbstractCamera::createProjectionMatrix(camera.getWidth(), camera.getHeight(), camera.getFov(),
		                                                                   tightNear, tightFar, true);
		m_tightCameraFrustum = GfxUtils::unprojectMatrix(tightProjection * camera.getView());
	}
}

std::vector<Ptr<GameObject>> ShadowMap::findShadowReceivers(const BoundingBox& box, Scene& scene)
{
	std::vector<Ptr<GameObject>> out;
	for (const auto& entity : scene.getEntities())
	{
		if (!entity->m_shadowReceiver)
			continue;

		// Assume every entity is a GameObject //TODO: (DR) Refactor with ECS
		Ptr<GameObject> gameObject = std::static_pointer_cast<GameObject>(entity);
		// TODO: (DR) This computation should be avoided

		// Apply model transform to the mesh AABB and then create a new AABB from transformed points to realign it
		BoundingBox meshAABB = {gameObject->m_mesh->m_boundingBoxMin, gameObject->m_mesh->m_boundingBoxMax};
		BoundingBox entityAABB = BoundingBox::createBoundingBox(&meshAABB.getTransformedPoints(gameObject->m_modelMatrix)[0], 8);
		gameObject->m_aabb = entityAABB;
		// Intersection test if entity hits frustum
		if (Collisions::intersect(entityAABB, box))
		{
			out.push_back(gameObject);
		}
	}
	return out;
}

std::pair<float, float> ShadowMap::findTightNearAndFarPlanes(const glm::vec3& origin, const glm::vec3& dir,
                                                             const std::vector<Ptr<GameObject>> objects)
{
	float minZ = FLT_MAX;
	float maxZ = 0;

	for (const auto& object : objects)
	{
		std::array<glm::vec3, 8> points = object->m_aabb.getPoints();
		for (const auto& point : points)
		{
			// Distance to plane defined by origin and direction
			float distance = glm::dot(dir, point - origin);
			if (distance < minZ)
				minZ = distance;
			if (distance > maxZ)
				maxZ = distance;
		}
	}
	return {minZ, maxZ};
}

} // namespace Dg