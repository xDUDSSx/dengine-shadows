
#include "TexturedObject.h"

#include "dengine/util/HSLColor.h"

#include "dengine/shader/ColorShader.h"
#include "dengine/shader/PhongShader.h"
#include "dengine/shader/Shaders.h"

using namespace Dg;

TexturedObject::TexturedObject(Dg::Mesh* mesh, PhongShader* shader) : GameObject(mesh, shader)
{
	// Empty
}

void TexturedObject::prepareRenderContext(Renderer::RenderContext& context)
{
	GameObject::prepareRenderContext(context);

	switch (context.m_renderType)
	{
		case Renderer::NORMAL:
			{
				PhongShader* phongShader = static_cast<PhongShader*>(context.m_shader);
				glm::vec3 color;
				glm::vec3 hsl;
				rgbToHsl(m_tint.r, m_tint.g, m_tint.b, &hsl.x, &hsl.y, &hsl.z);
				hsl.z = (hsl.z * m_tintStrength) + (1.0f - m_tintStrength);
				hslToRgb(hsl.x, hsl.y, hsl.z, &color.r, &color.g, &color.b);
				phongShader->m_tint = color;
			}
			break;
		case Renderer::SILHOUETTE:
			{
				auto colorShader = Shaders::instance().getShader<ColorShader>();
				colorShader->m_useSingleColor = true;
				colorShader->m_singleColor = m_highlightColor;
				context.m_shader = colorShader.get();
			}
			break;
		default:
			break;
	}
}
