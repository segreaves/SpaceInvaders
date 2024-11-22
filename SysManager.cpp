#include "SysManager.h"
#include "ActorManager.h"
#include "WindowManager.h"

SysManager::SysManager() :
	m_actorManager(nullptr),
	m_levelManager(nullptr)
{
	m_systems[SystemType::PlayerControl] = new Sys_PlayerControl(this);
	m_systems[SystemType::Health] = new Sys_Health(this);
	m_systems[SystemType::Movement] = new Sys_Movement(this);
	m_systems[SystemType::Rotation] = new Sys_Rotation(this);
	m_systems[SystemType::InvaderControl] = new Sys_InvaderControl(this);
	m_systems[SystemType::BulletControl] = new Sys_BulletControl(this);
	m_systems[SystemType::ShockwaveControl] = new Sys_ShockwaveControl(this);
	m_systems[SystemType::Collision] = new Sys_Collision(this);
	m_systems[SystemType::BunkerControl] = new Sys_BunkerControl(this);
	m_systems[SystemType::Renderer] = new Sys_Renderer(this);
	m_systems[SystemType::LevelState] = new Sys_LevelState(this);
	m_systems[SystemType::Sound] = new Sys_Sound(this);
	m_systems[SystemType::Spring] = new Sys_Spring(this);
	m_systems[SystemType::TorqueSpring] = new Sys_TorqueSpring(this);
	m_systems[SystemType::Gravity] = new Sys_Gravity(this);
	m_systems[SystemType::ShipSway] = new Sys_ShipSway(this);
	m_systemOrder = {
		SystemType::PlayerControl,
		SystemType::InvaderControl,
		SystemType::BulletControl,
		SystemType::BunkerControl,
		SystemType::ShockwaveControl,
		SystemType::Gravity,
		SystemType::Spring,
		SystemType::Movement,
		SystemType::ShipSway,
		SystemType::TorqueSpring,
		SystemType::Rotation,
		SystemType::Collision,
		SystemType::Sound,
		SystemType::Health,
		SystemType::LevelState,
		SystemType::Renderer
	};
}

SysManager::~SysManager()
{
	purgeSystems();
}

void SysManager::start()
{
	for (auto& [type, sys] : m_systems)
		sys->start();
}

void SysManager::update(const float& deltaTime)
{
	handleEvents();
	for (auto& type : m_systemOrder)
		m_systems[type]->update(deltaTime);
}

void SysManager::draw(WindowManager* windowManager)
{
	auto it = m_systems.find(SystemType::Renderer);
	if (it == m_systems.end()) return;
	Sys_Renderer* rend = (Sys_Renderer*)it->second;
	rend->draw(windowManager);
#ifdef DEBUG
	for (auto& sys : m_systems)
		sys.second->debugOverlay(windowManager);
#endif
}

void SysManager::handleEvents()
{
	for (auto& eventQueue : m_actorEvents)
	{
		EventId eventId = 0;
		while (eventQueue.second.size() > 0)
		{
			eventId = eventQueue.second.front();
			eventQueue.second.pop();
			for (auto& sys : m_systems)
				sys.second->handleEvent(eventQueue.first, (ActorEventType)eventId);
		}
	}
}

void SysManager::addEvent(ActorId actorId, ActorEvent eventId)
{
	m_actorEvents[actorId].push(eventId);
}

void SysManager::actorModified(const ActorId& actorId, const Bitmask& mask)
{
	for (auto& sys : m_systems)
	{
		if (sys.second->fitsRequirements(mask))
			sys.second->addActor(actorId);
		else
			sys.second->removeActor(actorId);
	}
}

void SysManager::removeActor(const ActorId& actorId)
{
	for (auto& sys : m_systems)
		sys.second->removeActor(actorId);
}

MessageHandler* SysManager::getMessageHandler()
{
	return &m_messageHandler;
}

void SysManager::purgeSystems()
{
	purgeActors();
	for (auto& sys : m_systems)
		delete sys.second;
	m_systems.clear();
}

void SysManager::purgeActors()
{
	for (auto& sys : m_systems)
		sys.second->purge();
}
