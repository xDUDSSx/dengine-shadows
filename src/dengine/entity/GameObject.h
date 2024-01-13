#pragma once

#include <memory>

#include "dengine/Resources/Mesh.h"
#include "Entity.h"

namespace Dg
{
class ObjectShader;

/**
 * \brief Regular world object entity. Represents a physical drawable object.
 */
class GameObject : public Entity
{
public:
	Core::Mesh* m_mesh;

	// TODO: (DR) Move this to entity I suppose
	float m_opacity{0.5f};

	/**
	 * Creates an empty GameObject. Before rendering a mesh and a shader need to
	 * be set!
	 */
	GameObject() = default;

	GameObject(Core::Mesh* mesh, ObjectShader* shader);

	void render(Shader* shader, glm::mat4 view, glm::mat4 projection, bool silhouette) override;
	void update(Scene& scene) override;
	void dispose() override;
};
} // namespace Dg