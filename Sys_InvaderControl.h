#pragma once
#include "Sys.h"
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
private:
	void selectTrackedInvaders();
	void instantiateShockwave(sf::Vector2f position);
	void increaseInvaderSpeed();
	void handleMovement(const float& deltaTime, const ActorId& id, Comp_Position* posComp, Comp_Movement* moveComp, Comp_Control* controlComp, Comp_Target* targetComp, Comp_Collision* colComp);
	void handleShooting(const float& deltaTime, const ActorId& id, Comp_Invader* invComp);

	LevelManager* m_levelManager;
	bool m_movingRight;
	float m_currentInvaderSpeed;
	unsigned int m_invaderBulletIndex;
	unsigned int m_shockwaveIndex;
	const float m_maxTargetDistance = 50.0f;
	ActorId m_leftInvader;
	ActorId m_rightInvader;
	const float m_dropDistance = 32.f;
	const float m_bounds = 10.f;
};
