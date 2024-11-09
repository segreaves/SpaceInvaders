#pragma once
#include "Sys.h"

class SysManager;

class Sys_Spring : public Sys
{
public:
	Sys_Spring(SysManager* systemManager);
	~Sys_Spring();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	sf::Vector2f calculateSpringForce(const sf::Vector2f& anchor, const sf::Vector2f& pos, const sf::Vector2f& vel, const float& strength, const float& length, const float& dampingCoeff);
	float calculateSpringTorque(const float& currentAngle, const float& targetAngle, const float& angularVelocity, const float& strength);
};