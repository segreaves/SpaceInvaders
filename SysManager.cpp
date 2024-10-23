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
	m_systems[SystemType::InvaderControl] = new Sys_InvaderControl(this);
	m_systems[SystemType::BulletControl] = new Sys_BulletControl(this);
	m_systems[SystemType::Shockwave] = new Sys_ShockwaveControl(this);
	m_systems[SystemType::Collision] = new Sys_Collision(this);
	m_systems[SystemType::BunkerControl] = new Sys_BunkerControl(this);
	m_systems[SystemType::Animator] = new Sys_Animator(this);
	m_systems[SystemType::Renderer] = new Sys_Renderer(this);
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
	for (auto& [type, sys] : m_systems)
		sys->update(deltaTime);
	handleEvents();
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
