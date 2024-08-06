#pragma once
#include "S_Base.h"
#include "MessageHandler.h"
#include "EventQueue.h"
#include <unordered_map>
#include <queue>

class EntityManager;

class SystemManager
{
public:
	SystemManager();
	~SystemManager();

	template<class T>
	T* getSystem(const System& systemType)
	{
		auto it = m_systems.find(systemType);
		return it != m_systems.end() ? dynamic_cast<T*> it->second : nullptr;
	}

	void addEvent(const EntityId& entity, const EventId& event);
	void handleEvents();

	EntityManager* getEntityManager() { return m_entityManager; }
	void setEntityManager(EntityManager* entityManager) { m_entityManager = entityManager; }
	MessageHandler* getMessageHandler() { return &m_messageHandler; }

	void purgeEntities();
	void purgeSystems();
private:
	std::unordered_map<System, S_Base*> m_systems;
	std::unordered_map<EntityId, EventQueue> m_entityEvents;
	EntityManager* m_entityManager;
	MessageHandler m_messageHandler;
};
