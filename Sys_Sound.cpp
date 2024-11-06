#include "Sys_Sound.h"
#include "SysManager.h"
#include "SoundType.h"
#include "WindowManager.h"

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
	auto posComp = m_systemManager->getActorManager()->getActor(msg.m_receiver)->getComponent<Comp_Position>(ComponentType::Position);
	std::string sound;
	switch ((SoundType)msg.m_int)
	{
		case SoundType::PlayerShoot:
			sound = "player_shoot";
			break;
		case SoundType::PlayerExplode:
			sound = "player_explode";
			break;
		case SoundType::InvaderExplode:
			sound = "invader_explode";
			break;
		case SoundType::InvaderShoot:
			sound = "invader_shoot";
			break;
	}
	m_systemManager->getActorManager()->getSoundManager()->play(sound);
}
