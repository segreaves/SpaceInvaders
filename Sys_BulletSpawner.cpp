#include "Sys_BulletSpawner.h"
#include "SysManager.h"

Sys_BulletSpawner::Sys_BulletSpawner(SysManager* systemManager) :
	Sys(systemManager),
	m_currentBullet(0)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_BulletSpawner::~Sys_BulletSpawner()
{
	unsubscribeFromChannels();
}

void Sys_BulletSpawner::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::BulletControl);
}

void Sys_BulletSpawner::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Shoot, this);
}

void Sys_BulletSpawner::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Shoot, this);
}

void Sys_BulletSpawner::update(const float& deltaTime)
{
}

void Sys_BulletSpawner::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_BulletSpawner::debugOverlay(WindowManager* windowManager)
{
}

void Sys_BulletSpawner::notify(const Message& msg)
{

}
