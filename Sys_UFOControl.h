#pragma once
#include "Sys.h"

class SysManager;

class Sys_UFOControl : public Sys
{
public:
	Sys_UFOControl(SysManager* systemManager);
	~Sys_UFOControl();
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
	void initializeTimer();
	void initializeUFO();
	void handleUFO(const float& deltaTime);
	void onUFODeath(const ActorId& id);
	bool m_ufoEnabled;
	sf::Vector2f m_startOffset;
	const float m_ufoSpeed = 100;
	bool m_movingRight;
	float m_ufoTimer;
	float m_ufoDuration;
	float m_soundTimer;
	float m_soundDuration = 0.1f;
	std::random_device m_rd;
	std::mt19937 m_gen; // Mersenne Twister engine
	std::uniform_real_distribution<float> m_unifFloat;
	std::uniform_int_distribution<int> m_unifInt;
};