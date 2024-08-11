#include "ActorManager.h"
#include "SysManager.h"
#include <iostream>

ActorManager::ActorManager(SysManager* systemManager) :
	m_systemManager(systemManager),
	m_idCounter(0)
{
	addComponentType<Comp_Position>(CompType::Position);
	addComponentType<Comp_Sprite>(CompType::Sprite);
}

ActorManager::~ActorManager()
{
	destroyAllActors();
}

int ActorManager::createActor(Bitmask components)
{
	Actor* actor = new Actor();
	for (int i = 0; i < components.size(); i++)
	{
		if (components.test(i))
		{
			CompType compType = (CompType)i;
			auto factory = m_componentFactory.find(compType);
			if (factory == m_componentFactory.end())
			{
				std::cout << "! Error: ActorManager::createActor() component factory element not found." << std::endl;
				return -1;
			}
			actor->addComponent(compType, factory->second());
		}
	}
	if (!m_actors.emplace(m_idCounter, actor).second)
	{
		std::cout << "! Error: ActorManager::createActor() failed to add new actor." << std::endl;
		delete actor;
		return -1;
	}
	m_systemManager->actorModified(m_idCounter, components);
	return m_idCounter++;
}

bool ActorManager::destroyActor(ActorId id)
{
	auto it = m_actors.find(id);
	if (it == m_actors.end()) return false;
	delete it->second;
	m_actors.erase(it);
	return true;
}

void ActorManager::destroyAllActors()
{
	while (m_actors.begin() != m_actors.end())
		destroyActor(m_actors.begin()->first);
}

Actor* ActorManager::getActor(ActorId id)
{
	auto it = m_actors.find(id);
	return it == m_actors.end() ? nullptr : it->second;
}
