
#include "GameObject.h"

#include "dengine/shader/ObjectShader.h"
#include "dengine/shader/Shaders.h"
#include "dengine/shader/DepthShader.h"

using namespace Dg;

GameObject::GameObject(Dg::Mesh* mesh, ObjectShader* shader) : m_mesh(mesh)
{
	m_shader = shader;
}

void GameObject::render(glm::mat4 view, glm::mat4 projection, const Renderer::RenderContext& context)
{
	// TODO: (DR) Refactor this once we switch to ECS

	Renderer::render(this, view, projection, context);

	//	auto objectShader = static_cast<ObjectShader*>(shader);
	//	objectShader->use();
	//	objectShader->m_wboit = m_wboit;
	//	objectShader->m_wboitFunc = m_wboitFunc;
	//	objectShader->m_opacity = m_opaque || silhouette ? 1.0f : m_opacity;
	//	objectShader->setWorldTransform(m_modelMatrix, view, projection);
	//	objectShader->setUniforms();
	//
	//	if (m_mesh)
	//	{
	//		glBindVertexArray(m_mesh->m_vao);
	//		for (auto& meshPart : m_mesh->m_meshParts)
	//		{
	//			objectShader->setUniformsPerMeshPart(meshPart);
	//			m_mesh->renderMeshPart(meshPart);
	//		}
	//		glBindVertexArray(0);
	//	}
}

void GameObject::prepareRenderContext(Renderer::RenderContext& context)
{
	switch (context.m_renderType)
	{
		case Renderer::SILHOUETTE:
		case Renderer::NORMAL:
			{
				if (context.m_shader == nullptr)
					context.m_shader = m_shader;

				context.m_opaque = this->m_opaque;
				context.m_opacity = this->m_opaque;
				context.m_wboit = this->m_wboit;
				context.m_wboitFunc = this->m_wboitFunc;
				break;
			}
		case Renderer::DEPTH:
			{
				DepthShader* depthShader = Shaders::instance().getShaderPtr<DepthShader>();
				context.m_shader = depthShader;
				break;
			}
		default:
			break;
	}
}

void GameObject::update(Scene& scene)
{
	// Empty
}

void GameObject::dispose()
{
	// Empty
}
