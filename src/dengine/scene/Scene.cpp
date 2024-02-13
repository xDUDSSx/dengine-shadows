
#include "Scene.h"

#include "dengine/platform/input/InputManager.h"
#include "dengine/resources/ResourceManager.h"
#include "dengine/platform/Logger.h"

#include "dengine/resources/Shaper.h"
#include "dengine/camera/OrbitCamera.h"
#include "dengine/shader/BoxBlurShader.h"
#include "dengine/shader/GridShader.h"
#include "dengine/shader/PhongShader.h"
#include "dengine/shader/ScreenOverlayShader.h"
#include "dengine/shader/SelectionCompositeShader.h"
#include "dengine/shader/WBOITCompositeShader.h"
#include "dengine/shader/DepthShader.h"
#include "dengine/renderer/Renderer.h"

#include "SceneRenderTarget.h"
#include "dengine/util/HSLColor.h"
#include "dengine/shader/Shaders.h"
#include "dengine/lights/ShadowMap.h"
#include "dengine/util/DebugDraw.h"
#include "dengine/util/Color.h"

namespace Dg
{

Scene::Scene()
{
	m_camera = std::make_shared<OrbitCamera>();
	m_lighting = std::make_shared<Lighting>();
	m_selectStencil = std::make_shared<SelectStencil>();
}

void Scene::draw(int width, int height, SceneRenderTarget& renderTarget, const DisplayOptions& displayOptions)
{
	m_camera->size(width, height);
	m_camera->update();

	if (renderTarget.getRenderOptions().shadows)
	{
		renderShadowMap(renderTarget, displayOptions);
	}

	return draw(width, height, m_camera->getView(), m_camera->getProjection(), renderTarget, displayOptions);
}

void Scene::renderShadowMap(SceneRenderTarget& renderTarget, const DisplayOptions& displayOptions)
{
	const RenderOptions& renderOptions = renderTarget.getRenderOptions();
	Ptr<ShadowMap> shadowMap = m_lighting->m_shadowSunLight.m_shadowMap;

	// Check for shadow map type changes
	if (shadowMap->m_shadowType != renderOptions.shadowType)
	{
		bool wasUsingPSSM = shadowMap->m_shadowType != Dg::RenderOptions::ShadowType::REGULAR;
		bool wantsToUsePSSM = renderOptions.shadowType != Dg::RenderOptions::ShadowType::REGULAR;
		if (wasUsingPSSM != wantsToUsePSSM)
		{
			// Recreate the shadow map fbo
			Ptr<Framebuffer> newShadowFBO = shadowMap->createShadowFramebuffer(renderOptions.shadowType);
			renderTarget.removeFramebuffer("shadows");
			renderTarget.addFramebuffer("shadows", newShadowFBO);

			// Reload phong shader with or without the PSSM define
			auto* phongShader = Shaders::instance().getShaderPtr<PhongShader>();
			LOG_INFO("Phong shader ID before reload: {}", phongShader->m_id);
			Shaders::reloadShader(*phongShader, phongShader->m_vertSource, phongShader->m_fragSource, phongShader->m_geoSource,
			                      wantsToUsePSSM ? "#define PSSM" : "");
		}
	}

	// Update shadow maps
	shadowMap->m_splitSchemeWeight = renderOptions.pssmShadowsSplitSchemeWeight;
	shadowMap->update(renderOptions.shadowType, *this, *m_camera);
	// Draw shadow maps
	shadowMap->drawShadowBuffer(renderTarget.getFramebuffer("shadows"), renderOptions, displayOptions);
}

void Scene::draw(int width, int height, glm::mat4 view, glm::mat4 projection, SceneRenderTarget& renderTarget,
                 const DisplayOptions& displayOptions)
{
	// TODO: (DR) This method seems a LITTLE too long, maybe cut it up into more methods or some outright renderer class?
	//   See note at the beginning of this file

	auto renderOptions = renderTarget.getRenderOptions();
	bool drawSelection = renderOptions.selection;
	bool alpha = renderOptions.framebufferAlpha;
	glm::vec3 clearColor = renderOptions.clearColor;
	bool multisample = renderOptions.multisample;
	unsigned int samples = renderOptions.samples;

	// Retrieve framebuffers from render target and try to update them with render settings
	Ptr<Framebuffer> mainFBO = renderTarget.getFramebuffer("main").lock();
	Ptr<Framebuffer> transparentFBO = renderTarget.getFramebuffer("transparent").lock();
	Ptr<Framebuffer> selectionFBO = nullptr;
	Ptr<Framebuffer> selectionBlurFBO = nullptr;
	Ptr<Framebuffer> selectionBlurSecondPassFBO = nullptr;
	if (drawSelection)
	{
		selectionFBO = renderTarget.getFramebuffer("selection").lock();
		selectionBlurFBO = renderTarget.getFramebuffer("selectionBlurFirstPass").lock();
		selectionBlurSecondPassFBO = renderTarget.getFramebuffer("selectionBlurSecondPass").lock();
	}
	Ptr<Framebuffer> shadowFBO;
	if (renderOptions.shadows)
	{
		shadowFBO = renderTarget.getFramebuffer("shadows").lock();
	}

	mainFBO->setMultisampled(multisample, samples);
	if (renderOptions.wboit)
	{
		transparentFBO->setMultisampled(multisample, samples);
	}
	if (drawSelection)
	{
		selectionFBO->setMultisampled(multisample, samples);
	}

	//	glm::mat4 lightMatrix{1.0f}; // TODO: Remove
	//	glm::vec3 lightPos{1.0f};

	// Draw the scene
	if (renderOptions.wboit)
	{
		////////
		// Weighted Blended Order Independent Transparency render (OpenGL 3 compatible)
		////////

		// ### 1. Draw opaque objects into a regular texture
		mainFBO->start(width, height);
		{
			// Using regular depth test and no blending
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);

			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilMask(0xFF);

			glClearColor(clearColor.r, clearColor.g, clearColor.b, alpha ? 0.0f : 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// Setup phong shader, later, shaders are switched for each object
			PhongShader* phongShader = Shaders::instance().getShaderPtr<PhongShader>();
			phongShader->m_lightingModel = static_cast<PhongShader::LightingModel>(renderOptions.lightingModel);
			phongShader->use();
			m_lighting->setUniforms(*phongShader);

			m_unorderedTransparentEntities.clear();
			for (auto& entity : m_entities)
			{
				if (!entity->m_visible)
					continue;
				if (!displayOptions.shouldDraw(*entity))
					continue;
				if (!entity->m_opaque)
				{
					m_unorderedTransparentEntities.push_back(entity.get());
				}
				else
				{
					if (entity->m_selectable)
					{
						glStencilMask(0xFF);
						glStencilFunc(GL_ALWAYS, entity->m_selectionId, 0xFF);
					}
					else
					{
						glStencilMask(0x00);
					}
					Renderer::RenderContext context = entity->createRenderContext();
					context.m_wboit = false; // Not using wboit for opaque pass
					entity->render(view, projection, context);
				}
			}
		}
		mainFBO->end(false);

		// ### 2. Draw transparent objects into accumulation and revealage buffers

		// Transparent fbo might not be initialized or resized yet, so ensure proper state here before depth copy
		transparentFBO->update(width, height);

		// Copy depth buffer from opaque pass to transparent pass
		// Also copy stencil buffer for selection
		// TODO: (DR) This could be potentially avoided by sharing the depth buffer with opaque fbo?
		// 	(Framebuffer class currently doesn't support sharing of buffers)
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFBO->getId());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, transparentFBO->getId());
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

		transparentFBO->setDrawBuffers({1, 2});
		transparentFBO->start(width, height);
		{
			glClear(GL_COLOR_BUFFER_BIT); // Only clear color as depth is copied from the main framebuffer

			// Using depth test but not doing depth writes, special wboit blending enabled
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);

			// Clear accumulation and revealage buffers to their respective initial values
			// Note that the buffers are actually mixed
			// Accumulation buffer holds RGB accumulation in its RGB components and revealage factor in its alpha.
			// Revealage buffer holds accumulation alpha in its single channel.
			glClearBufferfv(GL_COLOR, 1, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
			glClearBufferfv(GL_COLOR, 2, glm::value_ptr(glm::vec4(0.0f)));

			// Render transparent objects with their WBOIT flag enabled (enabled after opaque pass)
			for (auto& entity : m_unorderedTransparentEntities)
			{
				if (entity->m_selectable)
				{
					glStencilMask(0xFF);
					glStencilFunc(GL_ALWAYS, entity->m_selectionId, 0xFF);
				}
				else
				{
					glStencilMask(0x00);
				}
				Renderer::RenderContext context = entity->createRenderContext();
				context.m_wboit = true;
				context.m_wboitFunc = renderOptions.wboitFunc;
				entity->render(view, projection, context);
			}
		}
		transparentFBO->end(false);
		if (transparentFBO->isMultisampled())
		{
			transparentFBO->multisampleResolveColor(1); // Resolve accumulation buffer
			transparentFBO->multisampleResolveColor(2); // Resolve revealage buffer
		}

		// ### 3. Composite transparent pass onto opaque pass
		mainFBO->start(width, height);
		{
			// Depth test disabled, regular blending enabled
			glDepthFunc(GL_ALWAYS);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			auto wboitCompositeShader = Shaders::instance().getShaderPtr<WBOITCompositeShader>();
			wboitCompositeShader->use();
			wboitCompositeShader->accumulationTextureID = transparentFBO->getColorTexture(1);
			wboitCompositeShader->revealageTextureID = transparentFBO->getColorTexture(2);
			wboitCompositeShader->setUniforms();

			// Render transparent object overlay using a screen quad
			RMI.meshByAlias(Shaper::screenQuad)->render();
		}
		mainFBO->end(true);

		// Return final composite image
		renderTarget.setOutputFramebuffer(WPtr<Framebuffer>(mainFBO));
	}
	else
	{
		////////
		// Standard ordered transparency render
		////////

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);

		//		Ptr<ShadowMap> shadowMap = m_lighting->m_shadowSunLight.m_shadowMap;

		mainFBO->start(width, height);
		{
			glClearColor(clearColor.r, clearColor.g, clearColor.b, alpha ? 0.0f : 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// Setup phong shader, later, shaders are switched for each object
			PhongShader* phongShader = Shaders::instance().getShaderPtr<PhongShader>();
			phongShader->m_lightingModel = static_cast<PhongShader::LightingModel>(renderOptions.lightingModel);
			// Assign the rendered shadow map texture to phong shader
			phongShader->m_visualizeShadowMap = displayOptions.showDebugVisualizeShadowMap;
			phongShader->m_isUsingPSSM = renderOptions.shadowType != RenderOptions::ShadowType::REGULAR;
			phongShader->use();
			m_lighting->setUniforms(*phongShader);
			if (renderOptions.shadows)
			{
				phongShader->m_shadowMapId = shadowFBO->getDepthAttachment()->m_id;
				//			phongShader->m_lightMatrix = lightMatrix;
				//			phongShader->m_lightPos = lightPos;
				//			phongShader->m_lightView = lightMatrix;
				m_lighting->m_shadowSunLight.setUniforms(*phongShader, 0);
			}

			m_unorderedTransparentEntities.clear();
			m_explicitTransparencyOrderEntitiesFirst.clear();
			m_explicitTransparencyOrderEntitiesLast.clear();
			for (auto& entity : m_entities)
			{
				if (!entity->m_visible)
					continue;
				if (!displayOptions.shouldDraw(*entity))
					continue;

				// Render opaque entities
				if (entity->m_opaque)
				{
					if (entity->m_selectable)
					{
						glStencilMask(0xFF);
						glStencilFunc(GL_ALWAYS, entity->m_selectionId, 0xFF);
					}
					else
					{
						glStencilMask(0x00);
					}
					Renderer::RenderContext context = entity->createRenderContext();
					context.m_wboit = false; // Not using wboit for opaque pass
					entity->render(view, projection, context);
					continue;
				}
				// Store transparent entities for sorting
				if (entity->m_explicitTransparencyOrder != 0)
				{
					// Entities with manually set transparency order
					if (entity->m_explicitTransparencyOrder < 10000)
					{
						// Rendered BEFORE unordered ones
						m_explicitTransparencyOrderEntitiesFirst.push_back(entity.get());
					}
					else
					{
						// Rendered AFTER unordered ones
						m_explicitTransparencyOrderEntitiesLast.push_back(entity.get());
					}
				}
				else
				{
					// Unordered entities
					m_unorderedTransparentEntities.push_back(entity.get());
				}
			}

			// Sort transparent entities
			// Entities are grouped into 3 lists and drawn in this order
			// 1. Explicitly ordered entities (with manually set order) with order < 10000 (sorted in ascending order)
			// 2. Unordered entities (with order = 0) (Sorted by distance to camera)
			// 3. Explicitly ordered entities with order >= 10000 (sorted in ascending order)

			sortExplicitlyOrderedTransparentEntities(m_explicitTransparencyOrderEntitiesFirst);
			sortUnorderedTransparentEntities(view, m_unorderedTransparentEntities);
			sortExplicitlyOrderedTransparentEntities(m_explicitTransparencyOrderEntitiesLast);

			// Don't write depth, we want to see transparent entities through each other even when its wrong
			glDepthMask(GL_FALSE);

			renderSortedTransparentEntities(view, projection, m_explicitTransparencyOrderEntitiesFirst);
			renderSortedTransparentEntities(view, projection, m_unorderedTransparentEntities);
			renderSortedTransparentEntities(view, projection, m_explicitTransparencyOrderEntitiesLast);
		}

		if (displayOptions.showDebug)
		{
			Ptr<ShadowMap> shadowMap = m_lighting->m_shadowSunLight.m_shadowMap;
			DebugDraw::drawFrustum(shadowMap->m_lightProjection * shadowMap->m_lightView, Color::YELLOW, view, projection);
			DebugDraw::drawFrustum(shadowMap->m_croppedLightProjection * shadowMap->m_lightView, Color::TEAL, view, projection);

			DebugDraw::drawLineBox(shadowMap->m_testBox, Color::RED, view, projection);
			//		DebugDraw::drawLineBox(&shadowMap->m_testBox2[0], Color::BLACK, view, projection);
			DebugDraw::drawLineBox(GfxUtils::g_ndcPoints, Color::GREEN, view, projection);

			DebugDraw::drawLineBox(shadowMap->m_cameraFrustum.m_corners, glm::vec3(0.7f, 0.3f, 0.1f), view, projection);
			if (displayOptions.showDebugFrustumAABBs)
				DebugDraw::drawLineBox(shadowMap->m_cameraFrustumAABB, Color::ORANGE, view, projection);
			DebugDraw::drawLineBox(shadowMap->m_tightCameraFrustum.m_corners, Color::MAGENTA, view, projection);
			if (displayOptions.showDebugFrustumAABBs)
				DebugDraw::drawLineBox(shadowMap->m_tightCameraFrustumAABB, Color::WHITE, view, projection);

			const glm::vec3 colors[] = {Color::GREEN, Color::YELLOW, Color::RED, Color::BLUE};
			if (renderOptions.shadowType != RenderOptions::ShadowType::REGULAR)
			{
				for (int i = 0; i < PSSM_CASCADES; i++)
				{
					if (displayOptions.showDebugFrustums)
						DebugDraw::drawLineBox(shadowMap->m_splitFrustums[i].m_corners, colors[i] - glm::vec3(0.18f), view,
						                       projection);
					if (displayOptions.showDebugFrustumAABBs)
						DebugDraw::drawLineBox(shadowMap->m_splitFrustumsAABBs[i], colors[i] - glm::vec3(0.30f), view,
						                       projection);
					if (displayOptions.showDebugShadowMapVolumes)
					{
						glm::mat4 croppedLightProjection =
						    shadowMap->m_cropMatrices[i] * shadowMap->m_lightProjection * shadowMap->m_lightView;
						DebugDraw::drawFrustum(croppedLightProjection, colors[i], view, projection);
					}
				}
			}
			else
			{
				if (displayOptions.showDebugShadowMapVolumes)
				{
					glm::mat4 croppedLightProjection = shadowMap->m_lightPvmMatrices[0];
					DebugDraw::drawFrustum(croppedLightProjection, colors[0], view, projection);
				}
			}

			//		glm::vec4 testVec = glm::vec4(0.f, 0.f, 0.f, 1.0f);
			//		testVec = projection * view * testVec;
			//		GfxUtils::perspectiveDivide(testVec);
			//		LOG_INFO("Test vec clip: {}, {}, {}", testVec.x, testVec.y, testVec.z);

			for (const auto caster : shadowMap->m_debugCasters)
			{
				DebugDraw::drawLineBox(&(caster->m_aabb.getPoints()[0]), Color::TEAL, view, projection);
			}

			//			for (const auto receiver : shadowMap->m_receivers)
			//			{
			//				DebugDraw::drawLineBox(&(receiver->m_aabb.getPoints()[0]), Color::BLUE, view, projection);
			//			}
			//			for (const auto caster : shadowMap->m_casters)
			//			{
			//				DebugDraw::drawLineBox(&(caster->m_aabb.getPoints()[0]), glm::vec3(0.71f, 1.f, 0.f), view,
			// projection);
			//			}
		}

		mainFBO->end(true);

		// Return framebuffer
		renderTarget.setOutputFramebuffer(WPtr<Framebuffer>(mainFBO));
	}

	////////
	// Render selection / highlight
	////////

	// Note that the term "selection" and "highlight" is used interchangeably here
	if (drawSelection)
	{
		bool useDepth = renderOptions.highlight.useDepth;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Selection fbo might not be initialized or resized yet, so ensure proper state here before depth copy
		selectionFBO->update(width, height);

		if (useDepth)
		{
			// TODO: (DR) Why are we resolving it? We don't want it to be resolved if ther selection fbo is also
			// multisampled?! Add depth buffer sharing to fix this

			// Copy depth buffer from regular scene pass to selection pass, this might also resolve it
			// TODO: (DR) This could be potentially avoided by sharing the depth buffer with the main fbo?
			// 	(Framebuffer class currently doesn't support sharing of buffers)
			glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFBO->getId());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, selectionFBO->getId());
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		}

		// Do not render the highlight overlay if nothing is actually highlighted
		bool atLeastOneEntityHighlighted = false;

		// Do not render the highlight overlay if nothing is actually highlighted
		bool atLeastOneEntityHighlightUsesDepth = false;

		// ### 1. Draw entity silhouettes, save these silhouettes into the stencil buffer
		selectionFBO->start(width, height);
		{
			// #### 1.1 Regular silhouette render
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			// Don't clear depth as we copied it from the regular render pass
			glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glDepthFunc(GL_ALWAYS);
			glDepthMask(GL_FALSE);

			m_highlightedEntities.clear();
			for (auto& entity : m_entities)
			{
				entity->m_wboit = false; // Not using wboit since we don't care about transparency at all
				if (!entity->m_visible)
					continue;
				if (!displayOptions.shouldDraw(*entity))
					continue;
				if (!entity->m_highlight)
					continue;

				atLeastOneEntityHighlighted = true;
				if (entity->m_highlightUseDepth && useDepth)
				{
					atLeastOneEntityHighlightUsesDepth = true;
					m_highlightedEntities.push_back(entity.get());

					// Render the whole silhouette with the "covered" color, the uncovered portions will be drawn later.
					float darkenFactor = renderOptions.highlight.useDepth_darkenFactor;
					float saturationFactor = renderOptions.highlight.useDepth_desaturateFactor;
					auto coveredColor = HSLColor::fromRGB(glm::value_ptr(entity->m_highlightColor))
					                        .desaturate(saturationFactor)
					                        .darken(darkenFactor)
					                        .getRGB();
					auto originalColor = entity->m_highlightColor;

					Renderer::RenderContext context;
					context.m_renderType = Renderer::RenderType::SILHOUETTE;
					entity->m_highlightColor = glm::vec3(coveredColor[0], coveredColor[1], coveredColor[2]);
					entity->prepareRenderContext(context);
					context.m_wboit = false; // Not using wboit for opaque pass
					entity->render(view, projection, context);
					entity->m_highlightColor = originalColor;
				}
				else
				{
					// Simply render the silhouette with the desired highlight color
					Renderer::RenderContext context;
					context.m_renderType = Renderer::RenderType::SILHOUETTE;
					entity->prepareRenderContext(context);
					entity->render(view, projection, context);
				}
			}

			// #### 1.2 "Uncovered" depth-based silhouette render, render uncovered silhouette portions
			//  Note that many attempts were made to make this effect happen within a single pass using a shader, but
			//  ultimately this is the most simple and robust solution I've found.
			// 	Unfortunately with multisampled buffers it requires the selection fbos are multisampled as well in order
			//  to compare depth values properly.
			if (atLeastOneEntityHighlightUsesDepth && useDepth)
			{
				// Iterate over highlighted entities that use depth and render their uncovered portions
				glDepthFunc(GL_LEQUAL);
				for (auto& entity : m_highlightedEntities)
				{
					Renderer::RenderContext context;
					context.m_renderType = Renderer::RenderType::SILHOUETTE;
					entity->prepareRenderContext(context);
					entity->render(view, projection, context);
				}
			}
		}
		selectionFBO->end(true);

		if (atLeastOneEntityHighlighted)
		{
			auto boxBlurShader = Shaders::instance().getShaderPtr<BoxBlurShader>();
			int kernelSize = renderOptions.highlight.kernelSize;
			float blurFactor = renderOptions.highlight.downscaleFactor;
			int blurWidth = static_cast<int>(blurFactor * width);
			int blurHeight = static_cast<int>(blurFactor * height);

			// ### 2. Blur the render (1st vertical blur pass)
			// The blur render can be done at a lower resolution specified by the blurDivisor
			selectionBlurFBO->start(blurWidth, blurHeight);
			{
				glDisable(GL_BLEND);

				glClear(GL_COLOR_BUFFER_BIT);

				boxBlurShader->use();
				boxBlurShader->m_kernelSize = kernelSize;
				boxBlurShader->m_vertical = true; // Vertical blur
				boxBlurShader->m_sourceTextureId = selectionFBO->getColorTexture(0);
				boxBlurShader->m_resolution = glm::vec2(blurWidth, blurHeight);
				boxBlurShader->setUniforms();

				// Render first box blur pass using a screen quad
				RMI.meshByAlias(Shaper::screenQuad)->render();
			}
			selectionBlurFBO->end();

			// ### 3. Blur the render again (2nd horizontal blur pass)
			selectionBlurSecondPassFBO->start(blurWidth, blurHeight);
			{
				glDisable(GL_BLEND);

				glClear(GL_COLOR_BUFFER_BIT);

				boxBlurShader->use();
				boxBlurShader->m_vertical = false; // Horizontal blur
				boxBlurShader->m_sourceTextureId = selectionBlurFBO->getColorTexture(0);
				boxBlurShader->setUniforms();

				// Render second box blur pass using a screen quad
				RMI.meshByAlias(Shaper::screenQuad)->render();
			}
			selectionBlurSecondPassFBO->end();

			// ### 4. Render back into the selection fbo and use its stencil to mask out the selection
			selectionFBO->start(width, height);
			{
				glDisable(GL_BLEND);
				glDisable(GL_DEPTH_TEST);

				glClear(GL_COLOR_BUFFER_BIT); // Clear color only

				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);

				auto selectionCompositeShader = Shaders::instance().getShaderPtr<SelectionCompositeShader>();
				selectionCompositeShader->use();
				selectionCompositeShader->m_sourceTextureId = selectionBlurSecondPassFBO->getColorTexture(0);
				selectionCompositeShader->m_resolution = glm::vec2(width, height);
				selectionCompositeShader->m_cutoff = renderOptions.highlight.outlineCutoff;
				selectionCompositeShader->setUniforms();

				// Upscale selection buffer and apply stencil
				RMI.meshByAlias(Shaper::screenQuad)->render();

				// Restore stencil state (can mess up ImGui)
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
			}
			selectionFBO->end(true);

			// ### 5. Overlay the final selection fbo on top of the main render
			auto mainFBOResolved = mainFBO->getResolvedFramebuffer().lock();
			mainFBOResolved->start(width, height);
			{
				glEnable(GL_BLEND);

				auto screenOverlayShader = Shaders::instance().getShaderPtr<ScreenOverlayShader>();
				screenOverlayShader->use();
				screenOverlayShader->m_sourceTextureId = selectionFBO->getColorTexture(0);
				screenOverlayShader->setUniforms();

				// Render the selection overlay
				RMI.meshByAlias(Shaper::screenQuad)->render();
			}
			mainFBOResolved->end();

			renderTarget.setOutputFramebuffer(WPtr<Framebuffer>(mainFBOResolved));
		}
	}
}

Ptr<SceneRenderTarget> Scene::createRenderTarget(const RenderOptions& options)
{
	Ptr<SceneRenderTarget> renderTarget = std::make_shared<SceneRenderTarget>();
	renderTarget->setRenderOptions(options);

	Ptr<Framebuffer> mainFramebuffer = std::make_shared<Framebuffer>(100, 100, options.multisample, options.samples);
	GLint format = (options.framebufferAlpha ? GL_RGBA : GL_RGB);
	// Regular color attachment
	mainFramebuffer->addColorAttachment(
	    ColorAttachment(format, format, mainFramebuffer->getWidth(), mainFramebuffer->getHeight(), GL_UNSIGNED_BYTE));
	// Depth buffer which we can sample from
	mainFramebuffer->setDepthAttachment(DepthAttachment(true, mainFramebuffer->getWidth(), mainFramebuffer->getHeight(), false));
	//	mainFramebuffer->setDepthAttachment(
	//	    DepthAttachment(GL_DEPTH24_STENCIL8, true, mainFramebuffer->getWidth(), mainFramebuffer->getHeight()));
	renderTarget->addFramebuffer("main", mainFramebuffer);

	Ptr<Framebuffer> transparentFramebuffer =
	    std::shared_ptr<Framebuffer>(Framebuffer::createDefault(options.multisample, options.samples, options.framebufferAlpha));
	transparentFramebuffer->addColorAttachment(ColorAttachment(GL_RGBA16F, GL_RGBA, 100, 100, GL_HALF_FLOAT));
	transparentFramebuffer->addColorAttachment(ColorAttachment(GL_R16F, GL_RED, 100, 100, GL_HALF_FLOAT));
	renderTarget->addFramebuffer("transparent", transparentFramebuffer);

	if (options.selection)
	{
		Ptr<Framebuffer> selectionFBO = std::make_shared<Framebuffer>(100, 100, options.multisample, options.samples);
		// Ptr<Framebuffer> selectionFBO = std::make_shared<Framebuffer>(100, 100, false, 1);
		selectionFBO->setDepthAttachment(DepthAttachment(true, 100, 100, true));
		auto c1 = ColorAttachment(GL_RGBA16F, GL_RGBA, 100, 100, GL_HALF_FLOAT);
		c1.m_minFilter = GL_LINEAR;
		c1.m_magFilter = GL_LINEAR;
		c1.m_textureWrapS = GL_CLAMP_TO_EDGE;
		c1.m_textureWrapT = GL_CLAMP_TO_EDGE;
		selectionFBO->addColorAttachment(c1);
		renderTarget->addFramebuffer("selection", selectionFBO);

		Ptr<Framebuffer> selectionBlurFirstPassFBO = std::make_shared<Framebuffer>(100, 100, false, 1);
		auto c2 = ColorAttachment(GL_RGBA16F, GL_RGBA, 100, 100, GL_HALF_FLOAT);
		c2.m_textureWrapS = GL_CLAMP_TO_EDGE;
		c2.m_textureWrapT = GL_CLAMP_TO_EDGE;
		selectionBlurFirstPassFBO->addColorAttachment(c2);
		renderTarget->addFramebuffer("selectionBlurFirstPass", selectionBlurFirstPassFBO);

		Ptr<Framebuffer> selectionBlurSecondPassFBO = std::make_shared<Framebuffer>(100, 100, false, 1);
		auto c3 = ColorAttachment(GL_RGBA16F, GL_RGBA, 100, 100, GL_HALF_FLOAT);
		c3.m_minFilter = GL_LINEAR;
		c3.m_magFilter = GL_LINEAR;
		c3.m_textureWrapS = GL_CLAMP_TO_EDGE;
		c3.m_textureWrapT = GL_CLAMP_TO_EDGE;
		selectionBlurSecondPassFBO->addColorAttachment(c3);
		renderTarget->addFramebuffer("selectionBlurSecondPass", selectionBlurSecondPassFBO);
	}

	if (options.shadows)
	{
		Ptr<Framebuffer> shadowFBO = m_lighting->m_shadowSunLight.m_shadowMap->createShadowFramebuffer(options.shadowType);
		renderTarget->addFramebuffer("shadows", shadowFBO);
	}

	return renderTarget;
}

void Scene::sortUnorderedTransparentEntities(glm::mat4 view, std::vector<Entity*>& entities)
{
	// Sort by distance to camera
	glm::vec3 cameraPos(glm::inverse(view)[3]);

	auto sortByDistanceToCamera = [&](Entity* e1, Entity* e2) -> bool {
		glm::vec3 entity1Pos = glm::vec3(e1->m_modelMatrix[3][0], e1->m_modelMatrix[3][1], e1->m_modelMatrix[3][2]);
		glm::vec3 entity2Pos = glm::vec3(e2->m_modelMatrix[3][0], e2->m_modelMatrix[3][1], e2->m_modelMatrix[3][2]);
		const float e1dist = glm::distance(cameraPos, entity1Pos);
		const float e2dist = glm::distance(cameraPos, entity2Pos);
		return e1dist > e2dist;
	};
	std::sort(m_unorderedTransparentEntities.begin(), m_unorderedTransparentEntities.end(), sortByDistanceToCamera);
}
void Scene::sortExplicitlyOrderedTransparentEntities(std::vector<Entity*>& entities)
{
	auto sortByExplicitTransparencyOrder = [&](Entity* e1, Entity* e2) -> bool {
		return e1->m_explicitTransparencyOrder > e2->m_explicitTransparencyOrder;
	};

	std::sort(m_explicitTransparencyOrderEntitiesFirst.begin(), m_explicitTransparencyOrderEntitiesFirst.end(),
	          sortByExplicitTransparencyOrder);
}

void Scene::renderSortedTransparentEntities(glm::mat4 view, glm::mat4 projection, const std::vector<Entity*>& entities) const
{
	for (const auto& entity : entities)
	{
		if (entity->m_backFaceCull)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		if (entity->m_selectable)
		{
			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, entity->m_selectionId, 0xFF);
		}
		else
		{
			glStencilMask(0x00);
		}
		entity->render(view, projection, entity->createRenderContext());
		if (entity->m_backFaceCull)
		{
			glDisable(GL_CULL_FACE);
		}
	}
}

void Scene::update(double dt)
{
	for (auto& entity : m_entities)
	{
		entity->update(*this);
	}
}

void Scene::precalculateBoundingBoxes()
{
	for (const auto& entity : getEntities())
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

void Scene::processInput(double dt, glm::vec2 mousePos, glm::ivec2 windowSize)
{
	m_camera->processInput(dt, mousePos, windowSize);
}

void Scene::processSelection(SceneRenderTarget& renderTarget, glm::vec2 mousePos, glm::ivec2 windowSize)
{
	if (InputManager::isActionTriggered("viewport_select"))
	{
		// Check if the click is inside the actual viewport bounds
		if (!(mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < windowSize.x && mousePos.y < windowSize.y))
			return;

		uint8_t id = 0;

		std::shared_ptr<Framebuffer> framebuffer;
		if (renderTarget.getRenderOptions().wboit)
		{
			// Use stencil buffer of the transparent pass if wboit is enabled
			framebuffer = renderTarget.getFramebuffer("transparent").lock();
		}
		else
		{
			// Use the main FBO
			framebuffer = renderTarget.getFramebuffer("main").lock();
		}
		if (framebuffer->isMultisampled())
		{
			// glReadPixels only works on single sampled buffers, the stencil buffer must first be resolved if
			// multisampled
			framebuffer->multisampleResolveDepth();
		}
		auto resolvedFramebuffer = framebuffer->getResolvedFramebuffer().lock();
		glBindFramebuffer(GL_FRAMEBUFFER, resolvedFramebuffer->getId());
		glReadPixels(static_cast<GLint>(mousePos.x), static_cast<GLint>(windowSize.y - mousePos.y), 1, 1, GL_STENCIL_INDEX,
		             GL_UNSIGNED_BYTE, &id);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// LOG_INFO("SELECTION at {},{}. Clicked id: {}", mousePos.x, mousePos.y, id);

		Entity* previousSelectedEntity = m_selectedEntity;
		Entity* newlySelectedEntity = nullptr;
		if (id != 0)
		{
			for (auto& e : m_entities)
			{
				if (e->m_selectionId == id)
				{
					newlySelectedEntity = e.get();
					break;
				}
			}
			if (newlySelectedEntity == nullptr)
			{
				LOG_ERROR("Viewport: Failed to select entity id: {}", id);
			}
		}
		m_selectedEntity = newlySelectedEntity;

		// Select entity anywhere elsewhere
		triggerSelectionCallbacks(newlySelectedEntity);
	}
}

void Scene::triggerSelectionCallbacks(Entity* entity)
{
	for (const auto& callback : m_selectionCallbacks)
	{
		callback(entity);
	}
}

} // namespace Dg