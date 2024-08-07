#pragma once
#include "S_Base.h"
#include "MessageHandler.h"
#include "EventQueue.h"
#include "WindowManager.h"
#include <unordered_map>
#include <queue>

class EntityManager;

class SystemManager
{
public:
	SystemManager();
	~SystemManager();

	/* Note: de-couple the entity manager and system manager by
	creating an event for purging entities, then subscribing the
	system manager to this event, and having the entity manager
	dispatch this event rather than call the purge() function
	on the system manager. */

	void update(const float& deltaTime);
	void draw(WindowManager* windowManager);

	template<class T>
	T* getSystem(const System& sys)
	{
		auto system = m_systems.find(sys);
		return system != m_systems.end() ? dynamic_cast<T*>(system->second) : nullptr;
	}

	EntityManager* getEntityManager();
	void setEntityManager(EntityManager* entityManager);

	void entityModified(const EntityId& id, const Bitmask& mask);
	void removeEntity(const EntityId& id);

	void purgeSystems();
	void purgeEntities();
private:
	std::unordered_map<System, S_Base*> m_systems;
	EntityManager* m_entityManager;

};
