#include "ActorManager.h"
#include "SysManager.h"

ActorManager::ActorManager(SysManager* systemManager, TextureManager* textureManager) :
	m_systemManager(systemManager),
	m_textureManager(textureManager),
	m_idCounter(0)
{
	addComponentType<Comp_Position>(ComponentType::Position);
	addComponentType<Comp_Sprite>(ComponentType::Sprite);
	addComponentType<Comp_Movement>(ComponentType::Movement);
	addComponentType<Comp_Control>(ComponentType::Control);
	addComponentType<Comp_Collision>(ComponentType::Collision);
	addComponentType<Comp_Player>(ComponentType::Player);
	addComponentType<Comp_Invader>(ComponentType::Invader);
	addComponentType<Comp_Bullet>(ComponentType::Bullet);
	addComponentType<Comp_Bunker>(ComponentType::Bunker);
	addComponentType<Comp_Shockwave>(ComponentType::Shockwave);
}

ActorManager::~ActorManager()
{
	destroyAllActors();
}

int ActorManager::createActor(Bitmask components, std::string tag)
{
	Actor* actor = new Actor(m_idCounter, tag);
	for (int i = 0; i < components.size(); i++)
	{
		if (components.test(i))
		{
			ComponentType compType = (ComponentType)i;
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

	disableActor(m_idCounter);
	
	return m_idCounter++;
}

int ActorManager::initializeActor(std::string tag)
{
	Actor* actor = new Actor(m_idCounter, tag);
	if (!m_actors.emplace(m_idCounter, actor).second)
	{
		std::cout << "! Error: ActorManager::initializeActor() failed to add new actor." << std::endl;
		delete actor;
		return -1;
	}
	disableActor(m_idCounter);
	return m_idCounter++;
}

void ActorManager::addComponent(const ActorId& id, ComponentType compType)
{
	auto it = m_actors.find(id);
	if (it == m_actors.end())
	{
		std::cout << "! Error: ActorManager::addComponent() actor with id " << id << " not found." << std::endl;
		return;
	}
	auto factory = m_componentFactory.find(compType);
	if (factory == m_componentFactory.end())
	{
		std::cout << "! Error: ActorManager::addComponent() component factory element " << (unsigned int)compType << " not found." << std::endl;
		return;
	}
	it->second->addComponent(compType, factory->second());
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
	m_idCounter = 0;
}

void ActorManager::enableActor(const ActorId& id)
{
	m_systemManager->actorModified(id, *getActor(id)->getComponentBitmask());
}

void ActorManager::enableAllActors()
{
	for (auto& actor : m_actors)
		enableActor(actor.first);
}

void ActorManager::disableActor(const ActorId& id)
{
	m_systemManager->actorModified(id, Bitmask(0));
}

void ActorManager::disableAllActors()
{
	for (auto& actor : m_actors)
		disableActor(actor.first);
}

Actor* ActorManager::getActor(const ActorId& id)
{
	auto it = m_actors.find(id);
	return it == m_actors.end() ? nullptr : it->second;
}
