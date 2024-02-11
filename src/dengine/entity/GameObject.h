#pragma once

#include <memory>

#include "dengine/Resources/Mesh.h"
#include "Entity.h"
#include "dengine/util/BoundingBox.h"

namespace Dg
{
class ObjectShader;

/**
 * \brief Regular world object entity. Represents a physical drawable object.
 */
class GameObject : public Entity
{
  public:
	Dg::Mesh* m_mesh;
	BoundingBox m_aabb;

	// TODO: (DR) Move this to entity I suppose
	float m_opacity{0.5f};

	/**
	 * Creates an empty GameObject. Before rendering a mesh and a shader need to
	 * be set!
	 */
	GameObject() = default;

	GameObject(Dg::Mesh* mesh, ObjectShader* shader);

	void render(glm::mat4 view, glm::mat4 projection, const Renderer::RenderContext& context) override;
	void update(Scene& scene) override;
	void dispose() override;

	void prepareRenderContext(Renderer::RenderContext& context) override;
};
} // namespace Dg