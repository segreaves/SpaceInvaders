#pragma once
#include "ISubscriber.h"
#include "System.h"
#include "EntityEvent.h"
#include "EntityManager.h"
#include "ECS_Params.h"
#include <vector>

class SystemManager;

class S_Base : public ISubscriber
{
public:
	S_Base(System id, SystemManager* systemManager);
	virtual ~S_Base();

	virtual void update(const float& deltaTime) = 0;
	virtual void handleEvent(const EntityId& entityId, const EntityEvent& msg) = 0;

	bool addEntity(const EntityId& entity);
	bool removeEntity(const EntityId& entity);
	bool hasEntity(const EntityId& entity) const;

	bool fitsRequirements(const Bitmask& bits) const;
	void purge();
protected:
	System m_id;
	std::vector<EntityId> m_entities;
	std::vector<Bitmask> m_requirements;
	SystemManager* m_systemManager;
};
