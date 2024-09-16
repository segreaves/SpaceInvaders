#pragma once
#include "Sys.h"
#include "Trigger.h"
#include "LevelManager.h"

class LevelManager;

class Sys_InvaderControl : public Sys
{
public:
	Sys_InvaderControl(SysManager* systemManager);
	~Sys_InvaderControl();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void setLevelManager(LevelManager* levelManager);

	Trigger<sf::Vector2f> m_invaderDefeated;
private:
	void selectTrackedInvaders();

	LevelManager* m_levelManager;
	bool m_movingRight;
	const float m_maxTargetDistance = 50.0f;
	ActorId m_leftInvader;
	ActorId m_rightInvader;
};
