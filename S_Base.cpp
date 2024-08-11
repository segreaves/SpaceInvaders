#include "S_Base.h"
#include "ECS_Params.h"

S_Base::S_Base(SystemType id, SystemManager* systemManager)
{
	m_id = id;
	m_systemManager = systemManager;
}

S_Base::~S_Base()
{
	purge();
}

bool S_Base::addEntity(const EntityId& entity)
{
	if (hasEntity(entity)) return false;
	m_entities.emplace_back(entity);
	return true;
}

bool S_Base::removeEntity(const EntityId& entity)
{
	auto toRemove = std::find(m_entities.begin(), m_entities.end(), entity);
	if (toRemove == m_entities.end()) return false;
	m_entities.erase(toRemove);
	return true;
}

bool S_Base::hasEntity(const EntityId& entity) const
{
	return std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end();
}

bool S_Base::fitsRequirements(const Bitmask& bits) const
{
	return std::find(m_requirements.begin(), m_requirements.end(), bits) != m_requirements.end();
}

void S_Base::purge()
{
	m_entities.clear();
}
