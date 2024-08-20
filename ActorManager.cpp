#include "ActorManager.h"
#include "SysManager.h"

ActorManager::ActorManager(SysManager* systemManager) :
	m_systemManager(systemManager),
	m_idCounter(0)
{
	addComponentType<Comp_Position>(CompType::Position);
	addComponentType<Comp_Sprite>(CompType::Sprite);
	addComponentType<Comp_Movable>(CompType::Movable);
	addComponentType<Comp_PlayerControl>(CompType::PlayerControl);
	addComponentType<Comp_AIControl>(CompType::AIControl);
	addComponentType<Comp_Collision>(CompType::Collision);
	addComponentType<Comp_BulletControl>(CompType::BulletControl);
}

ActorManager::~ActorManager()
{
	destroyAllActors();
}

int ActorManager::createActor(Bitmask components, const bool& enabled)
{
	Actor* actor = new Actor(m_idCounter);
	for (int i = 0; i < components.size(); i++)
	{
		if (components.test(i))
		{
			CompType compType = (CompType)i;
			auto factory = m_componentFactory.find(compType);
			if (factory == m_componentFactory.end())
			{
				std::cout << "! Error: ActorManager::createActor() component factory element " << (unsigned int)compType << " not found." << std::endl;
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

	if (enabled)
		enableActor(m_idCounter);
	else
		disableActor(m_idCounter);
	
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
	{
		delete m_actors.begin()->second;
		m_actors.erase(m_actors.begin());
	}
}

void ActorManager::enableActor(const ActorId& id)
{
	m_systemManager->actorModified(id, *getActor(id)->getComponentBitmask());
}

void ActorManager::disableActor(const ActorId& id)
{
	m_systemManager->actorModified(id, Bitmask(0));
}

Actor* ActorManager::getActor(const ActorId& id)
{
	auto it = m_actors.find(id);
	return it == m_actors.end() ? nullptr : it->second;
}
