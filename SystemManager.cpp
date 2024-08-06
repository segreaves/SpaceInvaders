#include "SystemManager.h"
#include "EntityManager.h"

SystemManager::SystemManager()
{
}

SystemManager::~SystemManager()
{
	purgeSystems();
}

void SystemManager::addEvent(const EntityId& entity, const EventId& event)
{
	m_entityEvents[entity].addEvent(event);
}

void SystemManager::handleEvents()
{
	for (auto& entityEvent : m_entityEvents)
	{
		EventId eventId = 0;
		while (entityEvent.second.processEvent(eventId))
		{
			for (auto& system : m_systems)
				if (system.second->hasEntity(entityEvent.first))
					system.second->handleEvent(entityEvent.first, (EntityEvent)eventId);
		}
	}
}

void SystemManager::purgeEntities()
{
	for (auto& system : m_systems)
		system.second->purge();
}

void SystemManager::purgeSystems()
{
	for (auto& system : m_systems)
		delete system.second;
	m_systems.clear();
}
