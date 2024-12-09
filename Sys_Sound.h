#pragma once
#include "Sys.h"

class SysManager;
class SoundManager;

/// <summary>
/// Calls sounds when actors with sound components request them.
/// </summary>
class Sys_Sound : public Sys
{
	public:
	Sys_Sound(SysManager* systemManager);
	~Sys_Sound();

	void setSound(bool soundOn);
protected:
	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	bool m_soundOn = true;
};
