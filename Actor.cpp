#include "Actor.h"

void Actor::onCreate()
{
	// add sprite component
}

void Actor::onDestroy()
{
		removeAllComponents();
}

void Actor::update(const float& deltaTime)
{
	for (auto& comp : m_components)
		comp.second->update(deltaTime);
}

void Actor::addComponent(Comp* component, const Bitmask& mask)
{
	if (m_components.find(mask) != m_components.end()) return;
	m_components[mask] = component;
}

void Actor::removeComponent(const Bitmask& mask)
{
	if (m_components.find(mask) == m_components.end()) return;
	delete m_components[mask];
	m_components.erase(mask);
}

void Actor::removeAllComponents()
{
	for (auto& comp : m_components)
		delete comp.second;
	m_components.clear();
}
