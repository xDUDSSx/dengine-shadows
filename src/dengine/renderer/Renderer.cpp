#include "Renderer.h"

#include "dengine/entity/Entity.h"
#include "dengine/entity/GameObject.h"
#include "dengine/shader/ObjectShader.h"
#include "dengine/shader/Shaders.h"
#include "dengine/shader/DepthShader.h"

namespace Dg
{

// TODO: (DR) Maybe wrap some values in some kind of RenderContext that gets passed to the render methods

void Renderer::render(GameObject* entity, glm::mat4 view, glm::mat4 projection, const RenderContext& context)
{
	assert(entity->m_mesh != nullptr);

	assert(dynamic_cast<ObjectShader*>(context.m_shader) != nullptr);
	auto objectShader = static_cast<ObjectShader*>(context.m_shader);

	objectShader->use();
	objectShader->m_wboit = context.m_wboit;
	objectShader->m_wboitFunc = context.m_wboitFunc;
	objectShader->m_opacity = context.m_opaque || context.m_opacity;
	objectShader->setWorldTransform(entity->m_modelMatrix, view, projection);
	objectShader->setUniforms();

	glBindVertexArray(entity->m_mesh->m_vao);
	for (auto& meshPart : entity->m_mesh->m_meshParts)
	{
		objectShader->setUniformsPerMeshPart(meshPart);
		if (context.m_instanceCount <= 0)
		{
			entity->m_mesh->renderMeshPart(meshPart);
		}
		else
		{
			entity->m_mesh->renderMeshPartInstanced(meshPart, context.m_instanceCount);
		}
	}
	glBindVertexArray(0);
}

} // namespace Dg