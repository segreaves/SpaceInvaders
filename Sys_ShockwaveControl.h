#pragma once
#include "Sys.h"

/// <summary>
/// Controls the creation and duration of shockwaves from invader explosions as well
/// as the shockwave effect on actors.
/// </summary>
class Sys_ShockwaveControl : public Sys
{
public:
	Sys_ShockwaveControl(SysManager* systemManager);
	~Sys_ShockwaveControl();
protected:
	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
};