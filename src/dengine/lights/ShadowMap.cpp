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
	m_dir = glm::normalize(target - origin);
	m_width = width;
	m_zNear = zNear;
	m_zFar = zFar;
}

void ShadowMap::update(Scene& scene, AbstractCamera& camera)
{
	precalculateBoundingBoxes(scene);
	computeTightShadowFrustum(camera, scene);
	buildCropMatrix(m_tightCameraFrustumAABB);
	m_casters = findShadowCasters(m_tightCameraFrustumAABB, scene);
}

// void ShadowMap::buildCropMatrix(const Frustum& frustum)
//{
//	glm::mat4 lightPvm = m_lightProjection * m_lightView;
//	BoundingBox box = frustum.createProjectedAABB(lightPvm);
////	box.m_min.z = -1.0f; // Do not modify the near plane eg. "Extend" bounding box towards the original near plane
////	box.m_max.z = 1.0f;
//	m_testBox = box;
//	m_cropMatrix = glm::ortho(box.m_min.x, box.m_max.x, box.m_min.y, box.m_max.y, box.m_min.z, box.m_max.z);
//	m_croppedLightProjection = m_cropMatrix * m_lightProjection;
//}

BoundingBox projectBoxIntoNDC(const BoundingBox& box, const glm::mat4& projection)
{
	std::array<glm::vec3, 8> points = box.getPoints();
	std::array<glm::vec3, 8> transformedPoints{};
	for (int i = 0; i < 8; i++)
	{
		glm::vec4 transformedPoint = glm::vec4(points[i], 1.0f);
		transformedPoint = projection * transformedPoint;
		GfxUtils::perspectiveDivide(transformedPoint);
		transformedPoints[i] = glm::vec3(transformedPoint);
		transformedPoints[i].z *= -1; // Z axis has to be flipped due to some confusing reason
	}
	BoundingBox ndcBox = BoundingBox::createBoundingBox(&transformedPoints[0], 8);
	return ndcBox;
}

void ShadowMap::buildCropMatrix(const BoundingBox& box)
{
	glm::mat4 lightPvm = m_lightProjection * m_lightView;

	BoundingBox ndcBox = projectBoxIntoNDC(box, lightPvm);
	// Near plane is at 1.0f, far plane at -1.0f, again, flipped z-coords for reasons
	ndcBox.m_max.z = 1.0f; // Do not modify the near plane eg. "Extend" bounding box towards the original near plane

	m_testBox = ndcBox;
	m_cropMatrix = glm::ortho(ndcBox.m_min.x, ndcBox.m_max.x, ndcBox.m_min.y, ndcBox.m_max.y, ndcBox.m_max.z, ndcBox.m_min.z);
	m_croppedLightProjection = m_cropMatrix * m_lightProjection;
}

//void ShadowMap::buildCropMatrix(const BoundingBox& box)
//{
//	glm::mat4 lightPvm = m_lightProjection * m_lightView;
//
//	std::array<glm::vec3, 8> points = box.getPoints();
//	std::array<glm::vec3, 8> transformedPoints{};
//	m_testBox2.clear();
//	for (int i = 0; i < 8; i++)
//	{
//		glm::vec4 transformedPoint = glm::vec4(points[i], 1.0f);
//		transformedPoint = lightPvm * transformedPoint;
//		GfxUtils::perspectiveDivide(transformedPoint);
//		transformedPoints[i] = glm::vec3(transformedPoint);
//		transformedPoints[i].z *= -1; // Z axis has to be flipped due to some confusing reason
//		m_testBox2.push_back(transformedPoints[i]);
//	}
//	BoundingBox ndcBox = BoundingBox::createBoundingBox(&transformedPoints[0], 8);
//	// Near plane is at 1.0f, far plane at -1.0f, again, flipped z-coords for reasons
//	ndcBox.m_max.z = 1.0f; // Do not modify the near plane eg. "Extend" bounding box towards the original near plane
//	                       //	ndcBox.m_min.z = -1.0f;
//	m_testBox = ndcBox;
//	m_cropMatrix = glm::ortho(ndcBox.m_min.x, ndcBox.m_max.x, ndcBox.m_min.y, ndcBox.m_max.y, ndcBox.m_max.z, ndcBox.m_min.z);
//	m_croppedLightProjection = m_cropMatrix * m_lightProjection;
//}

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
		m_tightCameraFrustumAABB = m_tightCameraFrustum.createAABB();
	}
}

void ShadowMap::precalculateBoundingBoxes(Scene& scene)
{
	for (const auto& entity : scene.getEntities())
	{
		// Assume every entity is a GameObject //TODO: (DR) Refactor with ECS
		Ptr<GameObject> gameObject = std::static_pointer_cast<GameObject>(entity);
		// TODO: (DR) Bounding box should be precalculated somewhere else
		// Apply model transform to the mesh AABB and then create a new AABB from transformed points to realign it
		BoundingBox meshAABB = {gameObject->m_mesh->m_boundingBoxMin, gameObject->m_mesh->m_boundingBoxMax};
		BoundingBox entityAABB = BoundingBox::createBoundingBox(&meshAABB.getTransformedPoints(gameObject->m_modelMatrix)[0], 8);
		gameObject->m_aabb = entityAABB;
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

		// Intersection test if entity hits frustum
		if (Collisions::intersect(gameObject->m_aabb, box))
		{
			out.push_back(gameObject);
		}
	}
	return out;
}

std::vector<Ptr<GameObject>> ShadowMap::findShadowCasters(BoundingBox frustumAABB, Scene& scene)
{
	std::vector<Ptr<GameObject>> out;
	for (const auto& entity : scene.getEntities())
	{
		if (!entity->m_shadowCaster)
			continue;

		// Assume every entity is a GameObject //TODO: (DR) Refactor with ECS
		Ptr<GameObject> gameObject = std::static_pointer_cast<GameObject>(entity);

		if (Collisions::intersectSweep(gameObject->m_aabb, frustumAABB, m_dir))
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