#include "Entity.h"

Entity::Entity(unsigned int id)
{
	m_id = id;
}

bool Entity::addComponent(const Bitmask& componentType, C_Base* component)
{
	return m_components.emplace(std::make_pair(componentType, component)).second;
}

bool Entity::removeComponent(const Bitmask& component)
{
	auto itr = m_components.find(component);
	if (itr == m_components.end()) return false;

	delete itr->second;
	m_components.erase(itr);
	return true;
}

void Entity::onDestroy()
{
	for (auto& comp : m_components)
		delete comp.second;
	m_components.clear();
}
