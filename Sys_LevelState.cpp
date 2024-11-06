#include "Sys_LevelState.h"
#include "SysManager.h"
#include "SoundType.h"

Sys_LevelState::Sys_LevelState(SysManager* systemManager)
	: Sys(systemManager),
	m_deathTimer(0)
{
	onCreate();
}

Sys_LevelState::~Sys_LevelState()
{
	onDestroy();
}

void Sys_LevelState::start()
{
	m_playerDestroyed = false;
}

void Sys_LevelState::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Player);
	m_requirements.emplace_back(req);
}

void Sys_LevelState::subscribeToChannels()
{
}

void Sys_LevelState::unsubscribeFromChannels()
{
}

void Sys_LevelState::update(const float& deltaTime)
{
	if (m_playerDestroyed)
	{
		m_deathTimer -= deltaTime;
		if (m_deathTimer < 0)
			m_systemManager->getLevelManager()->setState(LevelState::PlayerDestroyed);
	}
}

void Sys_LevelState::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
		case ActorEventType::Invaded:
			m_systemManager->getLevelManager()->setState(LevelState::PlayerInvaded);
			break;
		case ActorEventType::Despawned:
			m_systemManager->getActorManager()->disableActor(actorId);
			m_playerDestroyed = true;
			m_deathTimer = m_gameOverWaitTime;
			break;
	}
}

void Sys_LevelState::debugOverlay(WindowManager* windowManager)
{
}

void Sys_LevelState::notify(const Message& msg)
{
}
