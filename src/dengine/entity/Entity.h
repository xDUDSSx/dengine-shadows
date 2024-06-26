#pragma once

#include "dengine/platform/DGL.h"
#include "dengine/renderer/Renderer.h"

namespace Dg
{
class Scene;
class Shader;

enum class DisplayType
{
	Default,
	Axes,
	Grid,
	Camera,
	Frustum
};

/**
 * \brief A scene entity base class.
 */
class Entity
{
  protected:
	DisplayType m_displayType{DisplayType::Default};

  public:
	glm::mat4 m_modelMatrix{1.f}; ///< Model transformation of the entity

	// Some basic properties
	bool m_backFaceCull;
	bool m_visible;
	bool m_opaque;
	bool m_wboit;                             ///< Whether this entity should be rendered using order independent transparency,
	                                          ///< relevant if using a shader that supports wboit
	int m_wboitFunc;                          ///< Index of the weight function to be used in wboit
	unsigned int m_explicitTransparencyOrder; ///< Explicit order for ordered transparency

	// Selection properties
	bool m_selectable;        ///< Whether this entity should be written into the selection stencil buffer
	int m_selectionId{-1};    ///< Id of the object, -1 if not assigned, for selection purposes
	bool m_highlight;         ///< Whether to paint this entity with an outline, can be used for selection or just highlighting
	bool m_highlightUseDepth; ///< Whether the highlight outline should respect the scene's depth buffer
	glm::vec3 m_highlightColor;

	Shader* m_shader{nullptr};

	// Shadow mapping
	bool m_shadowCaster{true};
	bool m_shadowReceiver{true};
	bool m_shadowCullFront{true};
	int m_shadowSplitBegin{INT_MAX};
	int m_shadowSplitEnd{INT_MIN};

	Entity();
	virtual ~Entity() = default;

	virtual void update(Scene& scene) = 0;

	/**
	 * IMPLEMENTATION NOTE:
	 * Derived entity classes should expect a certain kind of shader and be able to directly static cast to it.
	 * Currently there is no error checking and wrongly passed shader type will CAUSE UNDEFINED BEHAVIOUR!
	 *
	 * Using dynamic casts instead might be undesirable (slow) here (this might just be a premature optimalization)
	 * Generally the shader passed to this method is the m_shader of this entity.
	 * However we need functionality that allows us to change the shader temporarily (like for drawing silhouettes)
	 * Because of this the shader can also sometimes be something else, usually some derived type of the one the entity
	 * expects. I couldn't find a way to inform what kinds of shaders an entity is able to use in its render methods.
	 * The type of the shader must be the base Shader class and so some kind of casting must occur.
	 *
	 * Another option could be to use some kind of a typeid/enum system that tells us what type a shader is (rather than
	 * use dynamic cast). But that approach ignores inheritance which we need and having multiple types of shaders and
	 * entity member variable seems very inflexible.
	 *
	 * The true elegant solution probably doesn't involve inheritance at all and rather uses composition, together with
	 * an ECS or something. But it was decided to not dive into that realm and so we are stuck with this seemingly
	 * simple but not very flexible inheritance structure.
	 */
	virtual void render(glm::mat4 view, glm::mat4 projection, const Renderer::RenderContext& context) = 0;

	virtual void prepareRenderContext(Renderer::RenderContext& context) = 0;

	inline Renderer::RenderContext createRenderContext()
	{
		Renderer::RenderContext context;
		prepareRenderContext(context);
		return context;
	}

  public:
	virtual void dispose() = 0;

	/**
	 * Called when the entity is added to a scene.
	 * @param scene Scene to which it has been added
	 */
	virtual void onSceneAdd(Scene& scene) {}

	/**
	 * Called when the entity is removed from a scene.
	 * @param scene Scene from which it has been removed
	 */
	virtual void onSceneRemove(Scene& scene) {}

	virtual void setDisplayType(DisplayType type)
	{
		m_displayType = type;
	}
	virtual DisplayType getDisplayType() const
	{
		return m_displayType;
	}

	virtual void setHighlighted(bool highlight)
	{
		m_highlight = highlight;
	}
	virtual bool isHighlighted() const
	{
		return m_highlight;
	}

	virtual void setHighlightColor(glm::vec3 highlightColor)
	{
		m_highlightColor = highlightColor;
	}
	virtual glm::vec3 getHighlightColor() const
	{
		return m_highlightColor;
	}
};
} // namespace Dg