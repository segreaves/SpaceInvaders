#pragma once
#include "Sys.h"

class LevelManager;

class Sys_LevelState : public Sys
{
public:
	Sys_LevelState(SysManager* systemManager);
	~Sys_LevelState();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	void onPlayerDestroyed(ActorId id);

	float m_gameOverWaitTime = 2.f;
	float m_deathTimer;
	bool m_playerDestroyed = false;
};