#include "EntityManager.h"
#include "SystemManager.h"
#include <iostream>

EntityManager::EntityManager(SystemManager* systemManager) :
	m_idCounter(0),
	m_systemManager(systemManager)
{
	addComponentType<C_Position>(Component::Position);
	addComponentType<C_Sprite>(Component::Sprite);
}

EntityManager::~EntityManager()
{
	purge();
}

Entity* EntityManager::getEntity(const EntityId& id)
{
	return m_entities.find(id) != m_entities.end() ? m_entities[id] : nullptr;
}

int EntityManager::addEntity(const Bitmask& mask)
{
	//std::cout << "EntityManager::addEntity() Adding entity with mask: " << mask << std::endl;
	//std::cout << "Entity id: " << m_idCounter << std::endl;
	Entity* entity = new Entity(m_idCounter);
	for (unsigned int i = 0; i < n_components; i++)
	{
		if (mask.test(i))
		{
			Bitmask comp = Bitmask(1 << i);
			auto it = m_componentFactory.find(comp);
			if (it == m_componentFactory.end())
			{
				std::cerr << "! Error: Component type " << (1 << i) << " not found in factory when adding entity." << std::endl;
				delete entity;
				return -1;
			}
			entity->addComponent(comp, it->second());
		}
	}
	if (!m_entities.emplace(m_idCounter, entity).second)
	{
		std::cerr << "! Error: Adding entity id " << m_idCounter << std::endl;
		delete entity;
		return -1;
	}
	m_systemManager->entityModified(entity->getId(), mask);
	return m_idCounter++;
}

bool EntityManager::removeEntity(const EntityId& id)
{
	auto entity = m_entities.find(id);
	if (entity == m_entities.end()) return false;
	delete entity->second;
	m_entities.erase(entity);
	m_systemManager->removeEntity(id);
	return true;
}

void EntityManager::purge()
{
	m_systemManager->purgeEntities();
	for (auto& it : m_entities)
	{
		it.second->onDestroy();
		delete it.second;
	}
	m_entities.clear();
}
