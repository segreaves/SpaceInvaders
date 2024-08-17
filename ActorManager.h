#pragma once
#include "Actor.h"
#include "ECS_Params.h"
#include "Comp_Position.h"
#include "Comp_Sprite.h"
#include "Comp_PlayerControl.h"
#include "Comp_AIControl.h"
#include "Comp_Movable.h"
#include "Comp_Collision.h"
#include "ActorEventType.h"
#include <functional>

class SysManager;

using ActorId = unsigned int;

class ActorManager
{
public:
	ActorManager(SysManager* systemManager);
	~ActorManager();

	int createActor(Bitmask components);
	bool destroyActor(ActorId id);
	void destroyAllActors();

	Actor* getActor(ActorId id);

	template<class T>
	void addComponentType(CompType compType)
	{
		m_componentFactory[compType] = []()->Comp* { return new T(); };
	}
private:
	unsigned int m_idCounter;
	std::unordered_map<ActorId, Actor*> m_actors;
	std::unordered_map<CompType, std::function<Comp* (void)>> m_componentFactory;
	SysManager* m_systemManager;
};
