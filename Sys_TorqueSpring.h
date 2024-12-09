#pragma once
#include "Sys.h"

class SysManager;

/// <summary>
/// Simulates the behavior of a torque spring.
/// </summary>
class Sys_TorqueSpring : public Sys
{
public:
	Sys_TorqueSpring(SysManager* systemManager);
	~Sys_TorqueSpring();
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