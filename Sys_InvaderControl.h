#pragma once
#include "Sys.h"
#include "LevelManager.h"
#include <random>

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
private:
	void selectTrackedInvaders();
	void instantiateShockwave(sf::Vector2f position);
	void handleAITargetMovement(const float& deltaTime);
	void handleInvaderMovement(const float& deltaTime, const ActorId& id, std::shared_ptr<Comp_Position> posComp, std::shared_ptr<Comp_Movement> moveComp, std::shared_ptr<Comp_Target> targetComp, std::shared_ptr<Comp_Collision> colComp, std::shared_ptr<Comp_Invader> invComp, std::shared_ptr<Comp_Spring> springComp);
	void handleShooting(const float& deltaTime, const ActorId& id, std::shared_ptr<Comp_Invader> invComp);
	void onInvaderDeath(const ActorId& id);
	void setInvaderSpeed(const float& speed);

	sf::Vector2f m_aiTarget;
	bool m_movingRight;
	float m_aiSpeed;
	unsigned int m_invaderBulletIndex;
	sf::Text m_bulletCountText;
	sf::Font m_font;
	unsigned int m_shockwaveIndex;
	const float m_maxTargetDistance = 50.0f;
	ActorId m_leftInvader;
	ActorId m_rightInvader;
	const float m_dropDistance = 32.f;
	const float m_bounds = 10.f;

	std::random_device m_rd;
	std::mt19937 m_gen; // Mersenne Twister engine
	std::uniform_real_distribution<float> m_unifFloat;
	std::uniform_int_distribution<int> m_unifInt;
};
