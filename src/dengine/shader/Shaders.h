
#pragma once

#include <memory>
#include <unordered_map>

#include "dengine/platform/Common.h"

#include "dengine/resources/ResourceManager.h"
#include "dengine/shader/Shader.h"

namespace Dg
{
class ColorShader;
class PhongShader;
class DepthShader;
class GridShader;
class FrustumShader;
class WBOITCompositeShader;
class BoxBlurShader;
class SelectionCompositeShader;
class ScreenOverlayShader;

class Shaders
{
  public:
	static Shaders& instance()
	{
		static Shaders instance; // Guaranteed to be destroyed.
		                         // Instantiated on first use.
		                         // https://stackoverflow.com/a/1008289/3452003
		return instance;
	}

  private:
	Shaders() = default;

  public:
	Shaders(Shaders const&) = delete;
	void operator=(Shaders const&) = delete;

	bool loaded{false};

	bool load();
	bool reload();

	//	std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
	std::unordered_map<std::size_t, std::shared_ptr<Shader>> m_shaders;

	template <typename T, typename std::enable_if<std::is_base_of<Shader, T>::value, bool>::type = true>
	bool createShader(const std::string& vertSource)
	{
		return createShader<T>(vertSource, "");
	}

	template <typename T, typename std::enable_if<std::is_base_of<Shader, T>::value, bool>::type = true>
	bool createShader(const std::string& vertSource, const std::string& fragSource)
	{
		return createShader<T>(vertSource, fragSource, "");
	}

	template <typename T, typename std::enable_if<std::is_base_of<Shader, T>::value, bool>::type = true>
	bool createShader(const std::string& vertSource, const std::string& fragSource, const std::string& geoSource)
	{
		return createShader<T>(vertSource, fragSource, geoSource, "");
	}

	template <typename T, typename std::enable_if<std::is_base_of<Shader, T>::value, bool>::type = true>
	bool createShader(const std::string& vertSource, const std::string& fragSource, const std::string& geoSource,
	                  const std::string& sourceToInject)
	{
		std::size_t type = typeid(T).hash_code();
		if (m_shaders.contains(type))
		{
			LOG_ERROR("[SHADERS] Cannot create shader '{}'! A shader with that name already exists.");
			return false;
		}

		bool success;
		auto shader = loadShader<T>(success, vertSource, fragSource, geoSource, sourceToInject);
		if (!success)
		{
			LOG_ERROR("[SHADERS] Failed to create shader '{}'!");
			return false;
		}
		m_shaders[type] = shader;
		return true;
	}

	template <typename T>
	std::shared_ptr<T> getShader()
	{
		auto it = m_shaders.find(typeid(T).hash_code());
		if (it == m_shaders.end())
		{
			return nullptr;
		}
		return std::static_pointer_cast<T>(it->second);
	}

	template <typename T>
	T* getShaderPtr()
	{
		return getShader<T>().get();
	}

	static bool reloadShader(Shader& shader, const std::string& vertSource, const std::string& fragSource);
	static bool reloadShader(Shader& shader, const std::string& vertSource, const std::string& fragSource, const std::string& geoSource,
	                  const std::string& sourceToInject);

  private:
	template <typename T, typename std::enable_if<std::is_base_of<Shader, T>::value, bool>::type = true>
	std::shared_ptr<T> loadShader(bool& success, const std::string& vertSource, const std::string& fragSource)
	{
		return loadShader<T>(success, vertSource, fragSource, "");
	}

	template <typename T, typename std::enable_if<std::is_base_of<Shader, T>::value, bool>::type = true>
	std::shared_ptr<T> loadShader(bool& success, const std::string& vertSource, const std::string& fragSource,
	                              const std::string& geoSource)
	{
		return loadShader<T>(success, vertSource, fragSource, "", "");
	}

	template <typename T, typename std::enable_if<std::is_base_of<Shader, T>::value, bool>::type = true>
	std::shared_ptr<T> loadShader(bool& success, const std::string& vertSource, const std::string& fragSource,
	                              const std::string& geoSource, const std::string& sourceToInject)
	{
		GLuint id = RMI.shaderGI(vertSource, fragSource, geoSource, sourceToInject);
		if (id == 0)
		{
			success = false;
		}
		auto shader = std::make_shared<T>(id);
		shader->m_vertSource = vertSource;
		shader->m_fragSource = fragSource;
		shader->m_geoSource = geoSource;
		success = true;
		return shader;
	}

	bool checkForError(Shader& shader);
};

} // namespace Dg
