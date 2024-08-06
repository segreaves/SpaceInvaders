#pragma once
#include "ECS_Types.h"
#include "C_Base.h"
#include <vector>
#include <unordered_map>

class Entity
{
public:
	Entity(unsigned int id);

	unsigned int getId() const { return m_id; }

	bool addComponent(const Bitmask& componentType, C_Base* component);
	bool removeComponent(const Bitmask& component);

	template<class T>
	T* getComponent(const Bitmask& componentType)
	{
		if (m_components.find(componentType) == m_components.end()) return nullptr;
		return dynamic_cast<T*>(m_components[componentType]);
	}

	void onDestroy();
private:
	unsigned int m_id;
	std::unordered_map<Bitmask, C_Base*> m_components;
};
