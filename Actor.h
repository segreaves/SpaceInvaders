#pragma once
#include "Comp.h"
#include "ECS_Params.h"
#include <unordered_map>

class Actor
{
public:
	void onCreate();
	void onDestroy();

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
	std::unordered_map<CompType, Comp*> m_components;
	Bitmask m_componentBitmask;
};
