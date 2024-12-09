#pragma once
#include "Sys.h"

class LevelManager;

/// <summary>
/// Manages the state of the game. For example, when the player dies,
/// it tells the LevelManager to reset the game.
/// </summary>
class Sys_LevelState : public Sys
{
public:
	Sys_LevelState(SysManager* systemManager);
	~Sys_LevelState();
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
	float m_gameOverWaitTime = 2.f;
	float m_deathTimer;
	bool m_playerDestroyed = false;
};