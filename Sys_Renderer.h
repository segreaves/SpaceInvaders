#pragma once
#include "Sys.h"

class SysManager;
class ActorManager;
class WindowManager;

class Sys_Renderer : public Sys
{
public:
	Sys_Renderer(SysManager* systemManager);
	~Sys_Renderer();

	void setupRequirements() override;
	void subscribeToChannels() override;
	void unsubscribeFromChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void draw(WindowManager* windowManager);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
};