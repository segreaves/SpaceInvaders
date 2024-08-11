#pragma once
#include "System.h"

class SysManager;
class ActorManager;
class WindowManager;

class Sys_Renderer : public System
{
public:
	Sys_Renderer(SysManager* systemManager);

	void setupRequirements();
	void subscribeToChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEvent& msg);
	void draw(WindowManager* windowManager);

	void notify(const Message& msg);
};
