#pragma once
#include "Comp.h"
#include "ECS_Params.h"
#include <unordered_map>

class Actor
{
public:
	Actor(unsigned int id);
	~Actor();

	unsigned int getId();
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
	unsigned int m_id;
	std::unordered_map<CompType, Comp*> m_components;
	Bitmask m_componentBitmask;
};
