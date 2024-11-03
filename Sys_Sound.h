#pragma once
#include "Sys.h"

class SysManager;
class SoundManager;

class Sys_Sound : public Sys
{
	public:
	Sys_Sound(SysManager* systemManager);
	~Sys_Sound();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	sf::Sound m_sound;
};
