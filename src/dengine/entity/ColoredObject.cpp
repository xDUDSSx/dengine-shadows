
#include "ColoredObject.h"

using namespace Dg;

#include "dengine/shader/ColorShader.h"

ColoredObject::ColoredObject(Dg::Mesh* mesh, ColorShader* shader) : GameObject(mesh, shader)
{
	// Empty
}

void ColoredObject::prepareRenderContext(Renderer::RenderContext& context)
{
	GameObject::prepareRenderContext(context);

	switch (context.m_renderType)
	{
		case Renderer::SILHOUETTE:
		case Renderer::NORMAL:
			{
				// TODO: (DR) Assertion checking the shader type
				bool silhouette = context.m_renderType == Renderer::RenderType::SILHOUETTE;
				ColorShader* colorShader = static_cast<ColorShader*>(context.m_shader);
				colorShader->m_useSingleColor = m_useSingleColor || silhouette;
				colorShader->m_singleColor = silhouette ? m_highlightColor : m_singleColor;
			}
			break;
		default:
			break;
	}
}

const glm::vec3& ColoredObject::getColor() const
{
	return m_singleColor;
}

void ColoredObject::setColor(const glm::vec3* color)
{
	if (color == nullptr)
	{
		m_useSingleColor = false;
	}
	else
	{
		setColor(*color);
	}
}

void ColoredObject::setColor(const glm::vec3& mSingleColor)
{
	m_useSingleColor = true;
	m_singleColor = mSingleColor;
}
