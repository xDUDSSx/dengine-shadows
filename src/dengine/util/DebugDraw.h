#pragma once

#include "dengine/resources/Shaper.h"
#include "dengine/util/BoundingBox.h"
#include "dengine/entity/ColoredObject.h"
#include "dengine/shader/Shaders.h"
#include "dengine/shader/ColorShader.h"

namespace Dg
{

class DebugDraw
{
  public:
	static void drawLineBox(const BoundingBox& box, const glm::vec3& color, const glm::mat4& view, const glm::mat4& projection)
	{
		Shaper s;
		s.lineBox(box.m_min, box.m_max);
		Mesh* mesh = s.createLineMesh();
		ColoredObject* object = new ColoredObject(mesh, Shaders::instance().getShaderPtr<ColorShader>());
		object->m_useSingleColor = true;
		object->m_singleColor = color;
		Renderer::RenderContext context = object->createRenderContext();
		object->render(view, projection, context);
		delete mesh;
		delete object;
	}

	/**
	 * Draws a line box specified by the eight points.
	 * The point array order is following:
	 * First 4 points specify the -Z face from the -X,-Y corner counter clockwise
	 * Last 4 points specify the +Z face in the same order
	 *
	 * @param eightPoints Pointer to a point array of size exactly 8
	 * @param color Color of the lines
	 * @param view View matrix used for rendering
	 * @param projection Projection matrix used for rendering
	 */
	static void drawLineBox(const glm::vec3* eightPoints, const glm::vec3& color, const glm::mat4& view,
	                        const glm::mat4& projection)
	{
		Shaper s;
		s.lineBox(eightPoints);
		Mesh* mesh = s.createLineMesh();
		ColoredObject* object = new ColoredObject(mesh, Shaders::instance().getShaderPtr<ColorShader>());
		object->m_useSingleColor = true;
		object->m_singleColor = color;
		Renderer::RenderContext context = object->createRenderContext();
		object->render(view, projection, context);
		delete mesh;
		delete object;
	}

	static void drawFrustum(const glm::mat4& frustumTransform, const glm::vec3& color, const glm::mat4& view,
	                        const glm::mat4& projection)
	{
		glm::mat4 transformInv = glm::inverse(frustumTransform);
		glm::vec3 transformedPoints[8] = {};
		glm::vec3 ndcPoints[8] = {glm::vec3(-1, -1, -1), glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1), glm::vec3(1, -1, -1),
		                          glm::vec3(-1, -1, 1),  glm::vec3(-1, 1, 1),  glm::vec3(1, 1, 1),  glm::vec3(1, -1, 1)};
		for (int i = 0; i < 8; i++)
		{
			transformedPoints[i] = transformInv * glm::vec4(ndcPoints[i], 1.0f);
		}
		DebugDraw::drawLineBox(transformedPoints, glm::vec3(1, 1, 0), view, projection);
	}
};

} // namespace Dg
