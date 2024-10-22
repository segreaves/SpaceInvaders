#pragma once
#include "Comp.h"
#include "Params.h"
#include <unordered_map>

class Actor
{
public:
	Actor(const unsigned int id, std::string tag);
	~Actor();

	unsigned int getId();
	std::string getTag();
	void addComponent(ComponentType compType, std::shared_ptr<Comp> component);
	void removeComponent(ComponentType compType);
	void removeAllComponents();
	template<class T>
	std::shared_ptr<T> getComponent(ComponentType compType)
	{
		return m_components.find(compType) == m_components.end() ? nullptr : std::static_pointer_cast<T>(m_components[compType]);
	}
	void updateComponentBitmask();
	Bitmask* getComponentBitmask();
	bool isEnabled() { return m_enabled; }
	void setEnabled(bool enabled) { m_enabled = enabled; }
private:
	bool m_enabled;
	const unsigned int m_id;
	const std::string m_tag;
	std::unordered_map<ComponentType, std::shared_ptr<Comp>> m_components;
	Bitmask m_componentBitmask;
};
