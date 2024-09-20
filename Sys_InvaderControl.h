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

	Trigger<unsigned int> m_invaderDefeated;
	Trigger<unsigned int> m_invaderShoot;
private:
	void selectTrackedInvaders();
	void handleMovement(const float& deltaTime, const ActorId& id, Comp_Position* posComp, Comp_Movement* moveComp, Comp_Control* controlComp, Comp_Invader* invComp, Comp_Collision* colComp);
	void handleShooting(const float& deltaTime, const ActorId& id, Comp_Invader* invComp);

	LevelManager* m_levelManager;
	bool m_movingRight;
	const float m_maxTargetDistance = 50.0f;
	ActorId m_leftInvader;
	ActorId m_rightInvader;
	const float m_dropDistance = 32.f;
};
