#include "SystemManager.h"
#include "EntityManager.h"
#include "S_Renderer.h"

SystemManager::SystemManager() :
	m_entityManager(nullptr)
{
	m_systems[SystemType::Renderer] = new S_Renderer(this);
}

SystemManager::~SystemManager()
{
	purgeSystems();
}

void SystemManager::update(const float& deltaTime)
{
	for (auto& system : m_systems)
		system.second->update(deltaTime);
	handleEvents();
}

void SystemManager::draw(WindowManager* windowManager)
{
	auto it = m_systems.find(SystemType::Renderer);
	if (it == m_systems.end()) return;
	S_Renderer* renderer = (S_Renderer*)it->second;
	renderer->draw(windowManager);
}

EntityManager* SystemManager::getEntityManager()
{
	return m_entityManager;
}

void SystemManager::setEntityManager(EntityManager* entityManager)
{
	m_entityManager = entityManager;
}

MessageHandler* SystemManager::getMessageHandler()
{
	return &m_messageHandler;
}

void SystemManager::entityModified(const EntityId& id, const Bitmask& mask)
{
	for (auto& it : m_systems)
	{
		S_Base* system = it.second;
		if (system->fitsRequirements(mask))
			system->addEntity(id);
		else
			system->removeEntity(id);
	}
}

void SystemManager::removeEntity(const EntityId& id)
{
	for (auto& system : m_systems)
		system.second->removeEntity(id);
}

void SystemManager::addEvent(const EntityId& id, const EventId& event)
{
	m_entityEvents[id].addEvent(event);
}

void SystemManager::handleEvents()
{
	for (auto& eventQueue : m_entityEvents)
	{
		EventId eventId = 0;
		while (eventQueue.second.processEvent(eventId))
			for (auto& system : m_systems)
				if (system.second->hasEntity(eventQueue.first)) system.second->handleEvent(eventQueue.first, (EntityEvent)eventId);
	}
}

void SystemManager::notify(const Message& msg)
{
}

void SystemManager::purgeSystems()
{
	for (auto& system : m_systems)
		delete system.second;
	m_systems.clear();
}

void SystemManager::purgeEntities()
{
	for (auto& system : m_systems)
		system.second->purge();
}
