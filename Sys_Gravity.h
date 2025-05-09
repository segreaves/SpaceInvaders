#pragma once
#include "Sys.h"

class SysManager;

/// <summary>
/// Applies gravity to actors with mass.
/// </summary>
class Sys_Gravity : public Sys
{
public:
	Sys_Gravity(SysManager* systemManager);
	~Sys_Gravity();
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