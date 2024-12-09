#pragma once
#include "Sys.h"

class SysManager;

/// <summary>
/// Simulates the sway of a ship as it turns to the left or right.
/// </summary>
class Sys_ShipSway : public Sys
{
public:
	Sys_ShipSway(SysManager* systemManager);
	~Sys_ShipSway();
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
