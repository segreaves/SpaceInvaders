#pragma once
#include "Sys.h"

class SysManager;

/// <summary>
/// Handles all movement in the game. Applies acceleration, velocity, friction,
/// and damping.
/// </summary>
class Sys_Movement : public Sys
{
public:
	Sys_Movement(SysManager* systemManager);
	~Sys_Movement();
protected:
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void start();
	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
};