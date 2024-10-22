#include "Actor.h"

Actor::Actor(const unsigned int id, const std::string tag) :
	m_id(id),
	m_tag(tag)
{
}

Actor::~Actor()
{
	removeAllComponents();
}

unsigned int Actor::getId()
{
	return m_id;
}

std::string Actor::getTag()
{
	return m_tag;
}

void Actor::addComponent(ComponentType compType, std::shared_ptr<Comp> component)
{
	if (m_components.find(compType) != m_components.end()) return;
	m_components[compType] = component;
	updateComponentBitmask();
}

void Actor::removeComponent(ComponentType compType)
{
	if (m_components.find(compType) == m_components.end()) return;
	//delete m_components[compType];
	m_components.erase(compType);
	updateComponentBitmask();
}

void Actor::removeAllComponents()
{
	//for (auto& comp : m_components)
	//	delete comp.second;
	m_components.clear();
	updateComponentBitmask();
}

void Actor::updateComponentBitmask()
{
	m_componentBitmask.reset();
	for (auto comp : m_components)
		m_componentBitmask.set((unsigned int)comp.first);
}

Bitmask* Actor::getComponentBitmask()
{
	return &m_componentBitmask;
}
