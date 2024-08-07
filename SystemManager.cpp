#include "SystemManager.h"
#include "EntityManager.h"
#include "S_Renderer.h"

SystemManager::SystemManager() :
	m_entityManager(nullptr)
{
	m_systems[System::Renderer] = new S_Renderer(this);
}

SystemManager::~SystemManager()
{
	purgeSystems();
}

void SystemManager::update(const float& deltaTime)
{
	for (auto& system : m_systems)
		system.second->update(deltaTime);
}

void SystemManager::draw(WindowManager* windowManager)
{
	auto it = m_systems.find(System::Renderer);
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
