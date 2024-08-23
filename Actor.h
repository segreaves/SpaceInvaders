#pragma once
#include "Comp.h"
#include "ECS_Params.h"
#include <unordered_map>

class Actor
{
public:
	Actor(const unsigned int id, std::string tag);
	~Actor();

	unsigned int getId();
	std::string getTag();
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
	const unsigned int m_id;
	const std::string m_tag;
	std::unordered_map<CompType, Comp*> m_components;
	Bitmask m_componentBitmask;
};
