#include "Sys_Sound.h"
#include "SysManager.h"
#include "SoundType.h"

Sys_Sound::Sys_Sound(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_Sound::~Sys_Sound()
{
	onDestroy();
}

void Sys_Sound::start()
{
}

void Sys_Sound::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::SoundEmitter);
	m_requirements.emplace_back(req);
}

void Sys_Sound::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Sound, this);
}

void Sys_Sound::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Sound, this);
}

void Sys_Sound::update(const float& deltaTime)
{
}

void Sys_Sound::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Sound::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Sound::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	switch ((SoundType)msg.m_int)
	{
		case SoundType::PlayerShoot:
			m_systemManager->getActorManager()->getSoundManager()->play("player_shoot", false);
			break;
		case SoundType::PlayerExplode:
			m_systemManager->getActorManager()->getSoundManager()->play("player_explode", false);
			break;
	}
}
