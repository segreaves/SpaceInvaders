#pragma once
#include "Sys.h"

class SysManager;
class ActorManager;
class WindowManager;

class Sys_Renderer : public Sys
{
public:
	Sys_Renderer(SysManager* systemManager);

	void setupRequirements() override;
	void subscribeToChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEvent& msg);
	void draw(WindowManager* windowManager);

	void notify(const Message& msg);
};
