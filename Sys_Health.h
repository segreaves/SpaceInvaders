#pragma once
#include "Sys.h"

class LevelManager;

/// <summary>
/// Handles the player's health and taking damage.
/// </summary>
class Sys_Health : public Sys
{
public:
	Sys_Health(SysManager* systemManager);
	~Sys_Health();
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
	void handleBlinking(std::shared_ptr<Comp_SpriteSheet> spriteSheetComp, const float& deltaTime);

	const float m_maxDmgBlinkTime = 0.15f;
	const int m_dmgBlinkFrames = 7;
};