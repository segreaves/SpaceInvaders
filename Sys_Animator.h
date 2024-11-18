#pragma once
#include "Sys.h"

class SysManager;
class WindowManager;

/// <summary>
/// Handles the frame changes of all actors with a sprite sheet component.
/// </summary>
class Sys_Animator : public Sys
{
public:
	Sys_Animator(SysManager* systemManager);
	~Sys_Animator();
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
	void handleAnimation(std::shared_ptr<Comp_SpriteSheet> spriteSheetComp, const float& deltaTime);
};