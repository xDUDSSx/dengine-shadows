#include "ShadowMap.h"

#include <chrono>

#include "dengine/scene/Scene.h"
#include "dengine/framebuffer/Framebuffer.h"
#include "dengine/camera/AbstractCamera.h"
#include "dengine/util/Collisions.h"
#include "dengine/shader/PSSMShader.h"
#include "dengine/shader/PSSMInstancingShader.h"
#include "dengine/shader/Shaders.h"

using ShadowType = Dg::RenderOptions::ShadowType;

namespace Dg
{

void ShadowMap::setupShadowVolume(const glm::vec3& origin, const glm::vec3& target, float width, float zNear, float zFar)
{
	float hW = width / 2.0f;
	m_lightProjection = glm::orthoRH_NO(-hW, hW, -hW, hW, zNear, zFar);
	m_lightView = glm::lookAtRH(origin, target, glm::vec3(0.0f, 1.0f, 0.0f));
	m_origin = origin;
	m_target = target;
	m_dir = glm::normalize(target - origin);
	m_width = width;
	m_zNear = zNear;
	m_zFar = zFar;
}

void calculateSplitPositions(std::vector<float>& positions, int splitCount, float splitSchemeWeight, float zNear, float zFar)
{
	for (int i = 0; i < splitCount; i++)
	{
		float step = i / (float) splitCount;
		float log = zNear * powf(zFar / zNear, step);
		float uniform = zNear + (zFar - zNear) * step;
		positions[i] = log * splitSchemeWeight + uniform * (1 - splitSchemeWeight);
	}
	positions[0] = zNear;
	positions[splitCount] = zFar;
}

Frustum createSplitFrustum(float zNear, float zFar, const AbstractCamera& camera)
{
	glm::mat4 splitProjection =
	    AbstractCamera::createProjectionMatrix(camera.getWidth(), camera.getHeight(), camera.getFov(), zNear, zFar, true);
	return GfxUtils::unprojectMatrix(splitProjection * camera.getView());
}

Ptr<Framebuffer> ShadowMap::createShadowFramebuffer(ShadowType shadowType, int cascades, int resolution)
{
	m_shadowType = shadowType;
	// TODO: Shadow type switching
	Ptr<Framebuffer> shadowFBO;
	if (shadowType == ShadowType::REGULAR)
	{
		auto d = DepthAttachment(false, 100, 100, false);
		d.m_minFilter = GL_NEAREST;
		d.m_magFilter = GL_NEAREST;
		d.m_textureWrapS = GL_CLAMP_TO_BORDER;
		d.m_textureWrapT = GL_CLAMP_TO_BORDER;
		d.m_textureBorderColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
		shadowFBO = std::make_shared<Framebuffer>(resolution, resolution);
		shadowFBO->setDepthAttachment(d);
	}
	else
	{
		auto d = DepthAttachment(false, 100, 100, false);
		d.m_use2DTextureArray = true;
		d.m_2DTextureArrayLayers = cascades;
		d.m_minFilter = GL_NEAREST;
		d.m_magFilter = GL_NEAREST;
		d.m_textureWrapS = GL_CLAMP_TO_BORDER;
		d.m_textureWrapT = GL_CLAMP_TO_BORDER;
		d.m_textureBorderColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
		shadowFBO = std::make_shared<Framebuffer>(resolution, resolution);
		shadowFBO->setDepthAttachment(d);
	}
	return shadowFBO;
}

void ShadowMap::update(ShadowType shadowType, int cascadeCount, Scene& scene, AbstractCamera& camera)
{
	// TODO: Refactoring, some functions work with member state which is a mess

	// Assuming all GameObjects have up to date aabb bounding boxes!

	m_splitCount = cascadeCount;

	auto start = std::chrono::high_resolution_clock::now();

	// Adjust camera planes and create tightShadowFrustum
	computeTightShadowFrustum(camera, scene);

	if (m_shadowType == Dg::RenderOptions::ShadowType::REGULAR)
	{
		// A single shadow map that covers the tight camera frustum
		m_lightPvmMatrices.clear();
		m_cropMatrices.clear();

		m_casters.clear();

		// Find casters too, so we only render those, in order to compare this method reasonably to PSSM
		std::vector<GameObject*> localCasters = findShadowCasters(m_tightCameraFrustumAABB, 0, scene);
		m_casters.insert(localCasters.begin(), localCasters.end());

		glm::mat4 lightPvm = m_lightProjection * m_lightView;
		glm::mat4 cropMatrix = buildSceneInDependentCropMatrix(m_tightCameraFrustumAABB, lightPvm);
		m_cropMatrices.push_back(cropMatrix);
		m_lightPvmMatrices.push_back(cropMatrix * lightPvm);
	}
	else
	{
		// PSSM
		m_splitPositions.clear();
		m_splitPositions.resize(m_splitCount + 1);
		calculateSplitPositions(m_splitPositions, m_splitCount, m_splitSchemeWeight, m_zNearTight, m_zFarTight);

		m_casters.clear();
		m_splitFrustums.clear();
		m_splitFrustumsAABBs.clear();
		m_lightPvmMatrices.clear();
		m_lightPvmMatrices.resize(m_splitCount);
		m_cropMatrices.clear();
		m_cropMatrices.resize(m_splitCount);

		for (int i = 0; i < m_splitCount; i++)
		{
			Frustum splitFrustum = createSplitFrustum(m_splitPositions[i], m_splitPositions[i + 1], camera);
			m_splitFrustums.push_back(splitFrustum);
			BoundingBox splitFrustumAABB = splitFrustum.createAABB();
			m_splitFrustumsAABBs.push_back(splitFrustumAABB);
			std::vector<GameObject*> localCasters = findShadowCasters(splitFrustumAABB, i, scene);
			if (i == 0)
				m_debugCasters = localCasters;
			m_casters.insert(localCasters.begin(), localCasters.end());
			glm::mat4 lightPvm = m_lightProjection * m_lightView;
			glm::mat4 cropMatrix = buildSceneDependentCropMatrix(splitFrustumAABB, lightPvm, localCasters, m_receivers);
			m_cropMatrices[i] = cropMatrix;
			m_lightPvmMatrices[i] = cropMatrix * lightPvm;
		}
	}
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = duration_cast<std::chrono::microseconds>(stop - start);
	m_cpuUpdateTime = duration.count();
}

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

glm::mat4 ShadowMap::buildSceneInDependentCropMatrix(const BoundingBox& frustumAABB, const glm::mat4& lightPvm)
{
	// Simply build a light space bounding box around the frustum and push its near end to the near plane to capture all casters
	BoundingBox ndcBox = projectBoxIntoNDC(frustumAABB, lightPvm);
	// Near plane is at 1.0f, far plane at -1.0f, again, flipped z-coords for reasons
	ndcBox.m_max.z = 1.0f; // Do not modify the near plane eg. "Extend" bounding box towards the original near plane

	return glm::ortho(ndcBox.m_min.x, ndcBox.m_max.x, ndcBox.m_min.y, ndcBox.m_max.y, ndcBox.m_max.z, ndcBox.m_min.z);
}

glm::mat4 ShadowMap::buildSceneDependentCropMatrix(const BoundingBox& frustumAABB, const glm::mat4& lightPvm,
                                                   const std::vector<GameObject*>& casters,
                                                   const std::vector<Ptr<GameObject>>& receivers)
{
	BoundingBox ndcObjectsAABB;

	// TODO: Seemingly wrapping just the casters does the trick. This was mentioned in the paper.
	// Project each receiver and caster bounding box
	//	for (const auto& object : receivers)
	//	{
	//		ndcObjectsAABB.unionBox(projectBoxIntoNDC(object->m_aabb, lightPvm));
	//	}
	for (const auto& object : casters)
	{
		ndcObjectsAABB.unionBox(projectBoxIntoNDC(object->m_aabb, lightPvm));
	}

	BoundingBox ndcFrustumAABB = projectBoxIntoNDC(frustumAABB, lightPvm);

	// From the object and frustum aabb determine the final crop volume
	// The frustum's aabb is a limiting factor, the crop volume can't be bigger

	BoundingBox ndcBox;
	ndcBox.m_min.x = glm::max(ndcObjectsAABB.m_min.x, ndcFrustumAABB.m_min.x);
	ndcBox.m_max.x = glm::min(ndcObjectsAABB.m_max.x, ndcFrustumAABB.m_max.x);
	ndcBox.m_min.y = glm::max(ndcObjectsAABB.m_min.y, ndcFrustumAABB.m_min.y);
	ndcBox.m_max.y = glm::min(ndcObjectsAABB.m_max.y, ndcFrustumAABB.m_max.y);

	ndcBox.m_min.z = std::max(ndcObjectsAABB.m_min.z, ndcFrustumAABB.m_min.z);
	ndcBox.m_max.z = std::min(ndcObjectsAABB.m_max.z, 1.0f);

	if (casters.empty())
	{
		ndcBox = ndcFrustumAABB;
	}

	glm::mat4 cropMatrix =
	    glm::orthoRH_NO(ndcBox.m_min.x, ndcBox.m_max.x, ndcBox.m_min.y, ndcBox.m_max.y, ndcBox.m_max.z, ndcBox.m_min.z);
	return cropMatrix;
}

void ShadowMap::computeTightShadowFrustum(AbstractCamera& camera, Scene& scene)
{
	m_cameraFrustum = GfxUtils::unprojectMatrix(camera.getProjection() * camera.getView());
	m_cameraFrustumAABB = m_cameraFrustum.createAABB();
	m_receivers = findShadowReceivers(m_cameraFrustumAABB, scene);
	if (!m_receivers.empty())
	{
		auto tightNearFar = findTightNearAndFarPlanes(camera.getPosition(), camera.getDirection(), m_receivers);
		// Ensure that the tight near and far planes stay within original range, and that far is greater than near
		m_zNearTight = std::max(tightNearFar.first, camera.getZNear());
		m_zFarTight = std::max(tightNearFar.second, m_zNearTight + 0.01f);
		glm::mat4 tightProjection = AbstractCamera::createProjectionMatrix(camera.getWidth(), camera.getHeight(), camera.getFov(),
		                                                                   m_zNearTight, m_zFarTight, true);
		m_tightCameraFrustum = GfxUtils::unprojectMatrix(tightProjection * camera.getView());
		m_tightCameraFrustumAABB = m_tightCameraFrustum.createAABB();
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

std::vector<GameObject*> ShadowMap::findShadowCasters(BoundingBox frustumAABB, int splitIndex, Scene& scene)
{
	std::vector<GameObject*> out;
	for (const auto& entity : scene.getEntities())
	{
		if (!entity->m_shadowCaster)
			continue;

		// Assume every entity is a GameObject //TODO: (DR) Refactor with ECS
		Ptr<GameObject> gameObject = std::static_pointer_cast<GameObject>(entity);

		if (Collisions::intersectSweep(gameObject->m_aabb, frustumAABB, m_dir))
		{
			out.push_back(gameObject.get());

			// Update caster split range
			if (gameObject->m_shadowSplitBegin > splitIndex)
				gameObject->m_shadowSplitBegin = splitIndex;
			if (gameObject->m_shadowSplitEnd < splitIndex)
				gameObject->m_shadowSplitEnd = splitIndex;
		}
	}
	return out;
}

std::pair<float, float> ShadowMap::findTightNearAndFarPlanes(const glm::vec3& origin, const glm::vec3& dir,
                                                             const std::vector<Ptr<GameObject>>& objects)
{
	float minZ = std::numeric_limits<float>::max();
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
void ShadowMap::drawShadowBuffer(WPtr<Framebuffer> shadowFBOPtr, const RenderOptions& renderOptions,
                                 const DisplayOptions& displayOptions)
{
	// Assuming the passed shadowFBO contains textures setup according to the shadow map state

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);

	// Render shadow casters into shadow map
	Ptr<Framebuffer> shadowFBO = shadowFBOPtr.lock();
	shadowFBO->start(renderOptions.shadowResolution, renderOptions.shadowResolution);
	{
		glClear(GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT); // Cull front faces to try help with shadow artefacts

		PSSMShader* pssmShader = Shaders::instance().getShaderPtr<PSSMShader>();
		PSSMInstancingShader* pssmInstancingShader = Shaders::instance().getShaderPtr<PSSMInstancingShader>();

		switch (renderOptions.shadowType)
		{
			case RenderOptions::ShadowType::PSSM_GEO:
				pssmShader->cropMatrices = m_cropMatrices;
				break;
			case RenderOptions::ShadowType::PSSM_INSTANCED:
				pssmInstancingShader->cropMatrices = m_cropMatrices;
				break;
		}

		for (auto& shadowCaster : m_casters)
		{
			shadowCaster->m_wboit = false; // Not using wboit
			if (!shadowCaster->m_visible)
				continue;
			if (!displayOptions.shouldDraw(*shadowCaster))
				continue;

			Renderer::RenderContext context;
			switch (renderOptions.shadowType)
			{
				case RenderOptions::ShadowType::PSSM_GEO:
					context.m_renderType = Renderer::RenderType::CUSTOM;
					pssmShader->splitBegin = shadowCaster->m_shadowSplitBegin;
					pssmShader->splitEnd = shadowCaster->m_shadowSplitEnd;

					context.m_instanceCount = 0;
					context.m_shader = pssmShader;
					break;
				case RenderOptions::ShadowType::PSSM_INSTANCED:
					context.m_renderType = Renderer::RenderType::CUSTOM;
					pssmInstancingShader->splitBegin = shadowCaster->m_shadowSplitBegin;
					pssmInstancingShader->splitEnd = shadowCaster->m_shadowSplitEnd;

					// Render as many instances as the number of cascades the shadow caster is part of
					context.m_instanceCount = shadowCaster->m_shadowSplitEnd - shadowCaster->m_shadowSplitBegin + 1;
					context.m_shader = pssmInstancingShader;
					break;
				case RenderOptions::ShadowType::REGULAR:
					context.m_instanceCount = 0;
					context.m_renderType = Renderer::RenderType::DEPTH;
					// TODO: Implement simple shadow maps again
					break;
			}

			shadowCaster->prepareRenderContext(context);
			if (!shadowCaster->m_shadowCullFront)
				glDisable(GL_CULL_FACE);
			if (renderOptions.shadowType != RenderOptions::ShadowType::REGULAR)
			{
				// Vertices get the appropriate crop matrix applied in the vertex or geometry shader
				Renderer::render(shadowCaster, m_lightView, m_lightProjection, context);
			}
			else
			{
				// For regular shadow mapping we're cropping only once and so we can pass cropped projection directly
				Renderer::render(shadowCaster, m_lightView, m_cropMatrices[0] * m_lightProjection, context);
			}

			if (!shadowCaster->m_shadowCullFront)
				glEnable(GL_CULL_FACE);

			// Reset split indicators for next frame
			shadowCaster->m_shadowSplitBegin = INT_MAX;
			shadowCaster->m_shadowSplitEnd = INT_MIN;
		}
		glDisable(GL_CULL_FACE);
	}
	shadowFBO->end();
}

} // namespace Dg