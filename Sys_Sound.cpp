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
	if (!m_soundOn) return;
	std::string sound;
	switch ((SoundType)msg.m_int)
	{
		case SoundType::PlayerShoot:
			sound = "player_shoot";
			break;
		case SoundType::PlayerHit:
			sound = "player_hit";
			break;
		case SoundType::PlayerExplode:
			sound = "player_explode";
			break;
		case SoundType::InvaderSpawn:
			sound = "invader_spawn";
			break;
		case SoundType::InvaderExplode:
			sound = "invader_explode";
			break;
		case SoundType::InvaderShoot:
			sound = "invader_shoot";
			break;
		case SoundType::Beat:
			sound = "invader_beat";
			break;
		case SoundType::BulletHit:
			sound = "bullet_hit";
			break;
		case SoundType::UFOExplode:
			sound = "ufo_explode";
			break;
		case SoundType::UFOFly:
			sound = "ufo_fly";
			break;
		case SoundType::BunkerHit:
			sound = "bunker_hit";
			break;
	}
	const float& volume = msg.m_xy.x;
	const float& pitch = msg.m_xy.y;
	m_systemManager->getActorManager()->getSoundManager()->playSound(sound, volume, pitch);
}

void Sys_Sound::setSound(bool soundOn)
{
	m_soundOn = soundOn;
	if (!m_soundOn)
		m_systemManager->getActorManager()->getSoundManager()->turnOffSound();
}
