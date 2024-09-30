#pragma once
#include "Sys.h"

class SysManager;
class WindowManager;

class Sys_Animator : public Sys
{
public:
	Sys_Animator(SysManager* systemManager);
	~Sys_Animator();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	void handleAnimation(Comp_SpriteSheet* spriteSheetComp, const float& deltaTime);
	void handleBlinking(Comp_SpriteSheet* spriteSheetComp, const float& deltaTime);

	const float m_maxDmgBlinkTime = 0.15;
	const int m_dmgBlinkFrames = 7;
};