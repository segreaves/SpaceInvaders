#include "ActorManager.h"
#include "SysManager.h"

ActorManager::ActorManager(SysManager* systemManager, TextureManager* textureManager, SoundManager* soundManager) :
	m_systemManager(systemManager),
	m_textureManager(textureManager),
	m_soundManager(soundManager),
	m_idCounter(0)
{
	m_componentFactory.reserve(MAX_COMPONENTS);
	addComponentType<Comp_Position>(ComponentType::Position);
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
	addComponentType<Comp_Particles>(ComponentType::Particles);
	addComponentType<Comp_Grid>(ComponentType::Grid);
	addComponentType<Comp_SoundEmitter>(ComponentType::SoundEmitter);
	addComponentType<Comp_Spring>(ComponentType::Spring);
	addComponentType<Comp_Mass>(ComponentType::Mass);
	addComponentType<Comp_ShipSway>(ComponentType::ShipSway);
	addComponentType< Comp_TorqueSpring>(ComponentType::TorqueSpring);
}

ActorManager::~ActorManager()
{
	purge();
}

int ActorManager::initializeActor(std::string tag)
{
	auto actor = new Actor(m_idCounter, tag);
	if (!m_actors.emplace(m_idCounter, actor).second)
	{
		std::cout << "! Error: ActorManager::initializeActor() failed to add new actor." << std::endl;
		delete actor;
		return -1;
	}
	m_actorGroups[tag].emplace_back(m_idCounter);
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
	auto actorGroup = m_actorGroups[it->second->getTag()];
	auto it_2 = std::find(actorGroup.begin(), actorGroup.end(), id);
	m_actorGroups[it->second->getTag()].erase(it_2);
	it->second.reset();
	m_actors.erase(it);

	return true;
}

void ActorManager::purge()
{
	m_systemManager->purgeActors();
	m_actorGroups.clear();
	while (m_actors.begin() != m_actors.end())
	{
		m_actors.begin()->second.reset();
		m_actors.erase(m_actors.begin());
	}
	m_idCounter = 0;
}

void ActorManager::enableActor(const ActorId& id)
{
	getActor(id)->setEnabled(true);
	m_systemManager->actorModified(id, *getActor(id)->getComponentBitmask());
}

void ActorManager::enableAllActors()
{
	for (auto& actor : m_actors)
		enableActor(actor.first);
}

void ActorManager::disableActor(const ActorId& id)
{
	getActor(id)->setEnabled(false);
	m_systemManager->actorModified(id, Bitmask(0));
}

void ActorManager::disableAllActors()
{
	for (auto& actor : m_actors)
		disableActor(actor.first);
}

std::shared_ptr<Actor> ActorManager::getActor(const ActorId& id)
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
	auto actor = getActor(actorId);
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
			auto pos = actor->getComponent<Comp_Position>(ComponentType::Position);
			ss >> *pos;
		}
		else if (attr == "Movement")
		{
			addComponent(actorId, ComponentType::Movement);
			auto movement = actor->getComponent<Comp_Movement>(ComponentType::Movement);
			ss >> *movement;
		}
		else if (attr == "MoveTarget")
		{
			addComponent(actorId, ComponentType::Target);
			auto target = actor->getComponent<Comp_Target>(ComponentType::Target);
			ss >> *target;
		}
		else if (attr == "Control")
		{
			addComponent(actorId, ComponentType::Control);
			auto control = actor->getComponent<Comp_Control>(ComponentType::Control);
			ss >> *control;
		}
		else if (attr == "Collision")
		{
			addComponent(actorId, ComponentType::Collision);
			auto collision = actor->getComponent<Comp_Collision>(ComponentType::Collision);
			ss >> *collision;
		}
		else if (attr == "Player")
		{
			addComponent(actorId, ComponentType::Player);
			auto player = actor->getComponent<Comp_Player>(ComponentType::Player);
			ss >> *player;
		}
		else if (attr == "Invader")
		{
			addComponent(actorId, ComponentType::Invader);
			auto invader = actor->getComponent<Comp_Invader>(ComponentType::Invader);
			ss >> *invader;
		}
		else if (attr == "Bullet")
		{
			addComponent(actorId, ComponentType::Bullet);
			auto bullet = actor->getComponent<Comp_Bullet>(ComponentType::Bullet);
			ss >> *bullet;
		}
		else if (attr == "Bunker")
		{
			addComponent(actorId, ComponentType::Bunker);
			auto bunker = actor->getComponent<Comp_Bunker>(ComponentType::Bunker);
			ss >> *bunker;
		}
		else if (attr == "Shockwave")
		{
			addComponent(actorId, ComponentType::Shockwave);
			auto shockwave = actor->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
			ss >> *shockwave;
		}
		else if (attr == "Particles")
		{
			addComponent(actorId, ComponentType::Particles);
			auto particles = actor->getComponent<Comp_Particles>(ComponentType::Particles);
			ss >> *particles;
		}
		else if (attr == "SpriteSheet")
		{
			addComponent(actorId, ComponentType::SpriteSheet);
			auto spriteSheet = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
			ss >> *spriteSheet;
			spriteSheet->create(m_textureManager);
		}
		else if (attr == "Health")
		{
			addComponent(actorId, ComponentType::Health);
			auto health = actor->getComponent<Comp_Health>(ComponentType::Health);
			ss >> *health;
		}
		else if (attr == "Grid")
		{
			addComponent(actorId, ComponentType::Grid);
			auto grid = actor->getComponent<Comp_Grid>(ComponentType::Grid);
			ss >> *grid;
		}
		else if (attr == "SoundEmitter")
		{
			addComponent(actorId, ComponentType::SoundEmitter);
			auto sound = actor->getComponent<Comp_SoundEmitter>(ComponentType::SoundEmitter);
			ss >> *sound;
			sound->create(m_soundManager);
		}
		else if (attr == "Mass")
		{
			addComponent(actorId, ComponentType::Mass);
			auto mass = actor->getComponent<Comp_Mass>(ComponentType::Mass);
			ss >> *mass;
		}
		else if (attr == "Spring")
		{
			addComponent(actorId, ComponentType::Spring);
			auto spring = actor->getComponent<Comp_Spring>(ComponentType::Spring);
			ss >> *spring;
		}
		else if (attr == "TorqueSpring")
		{
			addComponent(actorId, ComponentType::TorqueSpring);
			auto torqueSpring = actor->getComponent<Comp_TorqueSpring>(ComponentType::TorqueSpring);
			ss >> *torqueSpring;
		}
		else if (attr == "ShipSway")
		{
			addComponent(actorId, ComponentType::ShipSway);
			auto sway = actor->getComponent<Comp_ShipSway>(ComponentType::ShipSway);
			ss >> *sway;
		}
		else
			std::cerr << "Unknown attribute: " << attr << std::endl;
	}
	file.close();
	m_systemManager->addEvent(actorId, (EventId)ActorEventType::Spawned);
	return actorId;
}
