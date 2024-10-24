#include "Sys_LevelState.h"
#include "SysManager.h"

Sys_LevelState::Sys_LevelState(SysManager* systemManager)
	: Sys(systemManager)
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
			onPlayerDestroyed(actorId);
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

void Sys_LevelState::onPlayerDestroyed(ActorId id)
{
	m_systemManager->getActorManager()->disableActor(id);
	// enable player explosion particle system
	ActorId explosionId = m_systemManager->getLevelManager()->getPlayerExplosionId();
	auto explosionPos = m_systemManager->getActorManager()->getActor(explosionId)->getComponent<Comp_Position>(ComponentType::Position);
	auto playerPos = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_Position>(ComponentType::Position);
	explosionPos->setPosition(playerPos->getPosition());
	auto particlesComp = m_systemManager->getActorManager()->getActor(explosionId)->getComponent<Comp_Particles>(ComponentType::Particles);
	particlesComp->getParticleSystem()->initialize();
	particlesComp->getParticleSystem()->setEmitterPosition(explosionPos->getPosition());
	particlesComp->getParticleSystem()->emitParticles();
	m_systemManager->getActorManager()->enableActor(explosionId);
}
