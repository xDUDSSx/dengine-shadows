#pragma once

#include "dengine/platform/CommonGL.h"

namespace Dg
{
class GameObject;
class Entity;
class ObjectShader;
class Shader;

class Renderer
{
  public:
	enum RenderType
	{
		NORMAL,
		DEPTH,
		CUSTOM,
		SILHOUETTE
	};

	struct RenderContext
	{
		RenderType m_renderType{NORMAL};

		/**
	 	 * Whether to use weighted blended transparency for output if supported
	 	 * \see supportsWboit()
		 */
		bool m_wboit{false};
		int m_wboitFunc{0};

		Shader* m_shader{nullptr};

		bool m_opaque{true};
		float m_opacity{1.0f};
	};

	static void render(GameObject* entity, glm::mat4 view, glm::mat4 projection, const RenderContext& context);

//  private:
//	static void render(GameObject* entity, ObjectShader* shader, glm::mat4 view, glm::mat4 projection, const RenderContext& context);
};

} // namespace Dg
