#include "ActorManager.h"
#include "SysManager.h"

ActorManager::ActorManager(SysManager* systemManager, TextureManager* textureManager) :
	m_systemManager(systemManager),
	m_textureManager(textureManager),
	m_idCounter(0)
{
	addComponentType<Comp_Position>(ComponentType::Position);
	addComponentType<Comp_Sprite>(ComponentType::Sprite);
	addComponentType<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	addComponentType<Comp_Movement>(ComponentType::Movement);
	addComponentType<Comp_Target>(ComponentType::Target);
	addComponentType<Comp_Control>(ComponentType::Control);
	addComponentType<Comp_Collision>(ComponentType::Collision);
	addComponentType<Comp_Player>(ComponentType::Player);
	addComponentType<Comp_Invader>(ComponentType::Invader);
	addComponentType<Comp_Bullet>(ComponentType::Bullet);
	addComponentType<Comp_Bunker>(ComponentType::Bunker);
	addComponentType<Comp_Shockwave>(ComponentType::Shockwave);
	addComponentType<Comp_Health>(ComponentType::Health);
	addComponentType<Comp_Rotation>(ComponentType::Rotation);
	addComponentType<Comp_Particles>(ComponentType::Particles);
}

ActorManager::~ActorManager()
{
	purge();
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
	m_systemManager->actorModified(m_idCounter, Bitmask(0));
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

void ActorManager::purge()
{
	m_systemManager->purgeActors();
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
	m_actorDisabled.dispatch(id);
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

unsigned int ActorManager::loadActorProfile(const std::string actorName, const std::string tag)
{
	std::string fullPath = Utils::getWorkingDirectory() + "assets/profiles/" + actorName + ".dat";
	std::fstream file;
	file.open(fullPath);
	if (!file.is_open())
	{
		std::cerr << "LevelManager failed to open file: " << fullPath << std::endl;
		return -1;
	}
	unsigned int actorId = initializeActor(tag);
	Actor* actor = getActor(actorId);
	std::string line;
	while (getline(file, line))
	{
		std::stringstream ss(line);
		std::string attr;
		ss >> attr;
		if (line[0] == '#') continue;
		if (attr == "Position")
		{
			addComponent(actorId, ComponentType::Position);
			Comp_Position* pos = actor->getComponent<Comp_Position>(ComponentType::Position);
			ss >> *pos;
		}
		else if (attr == "Movement")
		{
			addComponent(actorId, ComponentType::Movement);
			Comp_Movement* movement = actor->getComponent<Comp_Movement>(ComponentType::Movement);
			ss >> *movement;
		}
		else if (attr == "MoveTarget")
		{
			addComponent(actorId, ComponentType::Target);
			Comp_Target* target = actor->getComponent<Comp_Target>(ComponentType::Target);
			ss >> *target;
		}
		else if (attr == "Control")
		{
			addComponent(actorId, ComponentType::Control);
			Comp_Control* control = actor->getComponent<Comp_Control>(ComponentType::Control);
			ss >> *control;
		}
		else if (attr == "Collision")
		{
			addComponent(actorId, ComponentType::Collision);
			Comp_Collision* collision = actor->getComponent<Comp_Collision>(ComponentType::Collision);
			ss >> *collision;
		}
		else if (attr == "Player")
		{
			addComponent(actorId, ComponentType::Player);
			Comp_Player* player = actor->getComponent<Comp_Player>(ComponentType::Player);
			ss >> *player;
		}
		else if (attr == "Invader")
		{
			addComponent(actorId, ComponentType::Invader);
			Comp_Invader* invader = actor->getComponent<Comp_Invader>(ComponentType::Invader);
			ss >> *invader;
		}
		else if (attr == "Bullet")
		{
			addComponent(actorId, ComponentType::Bullet);
			Comp_Bullet* bullet = actor->getComponent<Comp_Bullet>(ComponentType::Bullet);
			ss >> *bullet;
		}
		else if (attr == "Bunker")
		{
			addComponent(actorId, ComponentType::Bunker);
			Comp_Bunker* bunker = actor->getComponent<Comp_Bunker>(ComponentType::Bunker);
			ss >> *bunker;
		}
		else if (attr == "Shockwave")
		{
			addComponent(actorId, ComponentType::Shockwave);
			Comp_Shockwave* shockwave = actor->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
			ss >> *shockwave;
		}
		else if (attr == "Particles")
		{
			addComponent(actorId, ComponentType::Particles);
			Comp_Particles* particles = actor->getComponent<Comp_Particles>(ComponentType::Particles);
			ss >> *particles;
		}
		else if (attr == "Sprite")
		{
			addComponent(actorId, ComponentType::Sprite);
			Comp_Sprite* sprite = actor->getComponent<Comp_Sprite>(ComponentType::Sprite);
			ss >> *sprite;
		}
		else if (attr == "SpriteSheet")
		{
			addComponent(actorId, ComponentType::SpriteSheet);
			Comp_SpriteSheet* spriteSheet = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
			ss >> *spriteSheet;
			spriteSheet->create(m_textureManager);
		}
		else if (attr == "Health")
		{
			addComponent(actorId, ComponentType::Health);
			Comp_Health* health = actor->getComponent<Comp_Health>(ComponentType::Health);
			ss >> *health;
		}
		else
			std::cerr << "Unknown attribute: " << attr << std::endl;
	}
	file.close();
	m_systemManager->addEvent(actorId, (EventId)ActorEventType::Spawned);
	return actorId;
}
