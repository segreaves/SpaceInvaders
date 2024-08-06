#pragma once
#include "ECS_Params.h"
#include "Component.h"
#include "C_Base.h"
#include "Entity.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>

using EntityId = unsigned int;
using Components = std::vector<C_Base*>;

class SystemManager;

class EntityManager
{
public:
	EntityManager(SystemManager* systemManager/*, TextureManager* textureManager*/);
	~EntityManager();

	Entity* getEntity(const EntityId& id);
	int addEntity(const Bitmask& mask);
	bool removeEntity(const EntityId& id);

	template <class T>
	void addComponentType(const Component& compType)
	{
		Bitmask mask((unsigned int)compType);
		m_componentFactory[mask] = []()->C_Base* { return new T(); };
	}

	void purge();
private:
	unsigned int m_idCounter;
	SystemManager* m_systemManager;
	std::unordered_map<EntityId, Entity*> m_entities;
	std::unordered_map<Bitmask, std::function<C_Base* (void)>> m_componentFactory;
};
