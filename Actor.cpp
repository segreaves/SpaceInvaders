#include "Actor.h"

void Actor::onCreate()
{
}

void Actor::onDestroy()
{
		removeAllComponents();
}

void Actor::addComponent(CompType compType, Comp* component)
{
	if (m_components.find(compType) != m_components.end()) return;
	m_components[compType] = component;
	updateComponentBitmask();
}

void Actor::removeComponent(CompType compType)
{
	if (m_components.find(compType) == m_components.end()) return;
	delete m_components[compType];
	m_components.erase(compType);
	updateComponentBitmask();
}

void Actor::removeAllComponents()
{
	for (auto& comp : m_components)
		delete comp.second;
	m_components.clear();
}

void Actor::updateComponentBitmask()
{
	m_componentBitmask = Bitmask(0);
	for (auto comp : m_components)
		m_componentBitmask |= (int)comp.first;
}

Bitmask* Actor::getComponentBitmask()
{
	return &m_componentBitmask;
}
