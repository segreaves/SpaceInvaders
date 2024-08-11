#pragma once
#include "S_Base.h"
#include "MessageHandler.h"
#include "EventQueue.h"
#include "WindowManager.h"
#include <unordered_map>
#include <queue>

class EntityManager;

class SystemManager : public ISubscriber
{
public:
	SystemManager();
	~SystemManager();

	void update(const float& deltaTime);
	void draw(WindowManager* windowManager);

	template<class T>
	T* getSystem(const SystemType& sys)
	{
		auto system = m_systems.find(sys);
		return system != m_systems.end() ? dynamic_cast<T*>(system->second) : nullptr;
	}

	EntityManager* getEntityManager();
	void setEntityManager(EntityManager* entityManager);
	MessageHandler* getMessageHandler();

	void entityModified(const EntityId& id, const Bitmask& mask);
	void removeEntity(const EntityId& id);

	void addEvent(const EntityId& id, const EventId& event);
	void handleEvents();
	void notify(const Message& msg);

	void purgeSystems();
	void purgeEntities();
private:
	std::unordered_map<SystemType, S_Base*> m_systems;
	EntityManager* m_entityManager;
	MessageHandler m_messageHandler;
	std::unordered_map<EntityId, EventQueue> m_entityEvents;
};
