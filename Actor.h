#pragma once
#include "Comp.h"
#include "ECS_Params.h"
#include <unordered_map>

class Actor
{
public:
	Actor();
	~Actor();

	void enable();
	void disable();
	bool isEnabled();

	void addComponent(CompType compType, Comp* component);
	void removeComponent(CompType compType);
	void removeAllComponents();
	template<class T>
	T* getComponent(CompType compType)
	{
		return m_components.find(compType) == m_components.end() ? nullptr : dynamic_cast<T*>(m_components[compType]);
	}
	void updateComponentBitmask();
	Bitmask* getComponentBitmask();
private:
	// add component to actorManager so that it can be notified of enabled/disabled actors
	bool m_enabled;
	std::unordered_map<CompType, Comp*> m_components;
	Bitmask m_componentBitmask;
};
