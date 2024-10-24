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

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void draw(WindowManager* windowManager);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void draw(WindowManager* windowManager, std::shared_ptr<IDrawable> drawable);
};
