#pragma once
#include "Sys.h"
#include "LevelManager.h"
#include <random>

class LevelManager;

/// <summary>
/// Handle invader behavior such as movement direction and shooting.
/// Selects outter-most invaders to track and check for out-of-bounds.
/// </summary>
class Sys_InvaderControl : public Sys
{
public:
	Sys_InvaderControl(SysManager* systemManager);
	~Sys_InvaderControl();
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
	void selectTrackedInvaders();
	void instantiateShockwave(sf::Vector2f position);
	void handleAITargetMovement(const float& deltaTime);
	void updateMoveTarget(const float& deltaTime, const ActorId& id, std::shared_ptr<Comp_Target> targetComp, std::shared_ptr<Comp_Invader> invComp);
	void checkBounds(const float& deltaTime, const ActorId& id, std::shared_ptr<Comp_Target> targetComp, std::shared_ptr<Comp_Collision> colComp);
	void tryShooting(const float& deltaTime, const ActorId& id, std::shared_ptr<Comp_Invader> invComp);
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
	const float m_bounds = 20.f;

	std::random_device m_rd;
	std::mt19937 m_gen; // Mersenne Twister engine
	std::uniform_real_distribution<float> m_unifFloat;
	std::uniform_int_distribution<int> m_unifInt;
};
