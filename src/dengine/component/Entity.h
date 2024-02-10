#pragma once

#include <typeinfo>
#include <list>
#include <unordered_map>
#include <stdexcept>

#include "dengine/platform/Common.h"

namespace DgTest
{

using ComponentType = size_t;
using ComponentIndex = unsigned int;

struct Component
{
	virtual ~Component() = default;
};

class ComponentManagerBase {
  public:
	virtual ~ComponentManagerBase() = default;
};

template <class C, IS_BASE_OF(Component, C)>
class ComponentManager : public ComponentManagerBase
{
  private:
	std::vector<C> m_components;

  public:
	ComponentIndex addComponent(C&& comp)
	{
		m_components.push_back(std::forward<C>(comp));
		return m_components.size() - 1;
	}

	template <class... Args>
	ComponentIndex createComponent(Args&&... args)
	{
		m_components.emplace_back(std::forward<Args>(args)...);
		return m_components.size() - 1;
	}

	C& getComponent(ComponentIndex idx)
	{
		return static_cast<C&>(m_components[idx]);
	}
};

class Entity
{
  public:
//	std::list<std::pair<ComponentType, ComponentIndex>> m_components;
	std::unordered_map<ComponentType, ComponentIndex> m_components;
	std::unordered_map<ComponentType, std::unique_ptr<ComponentManagerBase>> m_managers;

	template <class C, IS_BASE_OF(Component, C)>
	C& getComponent() {
		ComponentType type = getComponentType<C>();
		auto it = m_components.find(type);
		if (it == m_components.end())
		{
			throw std::runtime_error(std::string("Component '") + typeid(C).name() + "' does not exist!");
		}
		return getManager<C>(type).getComponent(it->second);
	}

	template <class C, IS_BASE_OF(Component, C)>
	void addComponent(C&& comp)
	{
		ComponentType type = getComponentType<C>();
		ComponentManager<C>& manager = getManager<C>(type);
		ComponentIndex compIdx = manager.addComponent(std::forward<C>(comp));
		m_components.emplace(type, compIdx);
	}

	template <class C, IS_BASE_OF(Component, C), class... Args>
	void createComponent(Args&&... args)
	{
		ComponentType type = getComponentType<C>();
		ComponentManager<C>& manager = getManager<C>(type);
		ComponentIndex compIdx = manager.createComponent(std::forward<Args>(args)...);
		m_components.emplace(type, compIdx);
	}

	template <class C, IS_BASE_OF(Component, C)>
	void registerComponent() {
		m_managers.insert(std::make_pair(typeid(C).hash_code(), std::make_unique<ComponentManager<C>>()));
	}

	template <class C, IS_BASE_OF(Component, C)>
	ComponentManager<C>& getManager(ComponentType type)
	{
		auto it = m_managers.find(type);
		if (it == m_managers.end())
		{
			throw std::runtime_error(std::string("Component '") + typeid(C).name() + "' was not registered!");
		}
		return *static_cast<ComponentManager<C>*>(it->second.get());
	}

	template <class C, IS_BASE_OF(Component, C)>
	ComponentType getComponentType()
	{
		return typeid(C).hash_code();
	}
};

} // namespace Dg
