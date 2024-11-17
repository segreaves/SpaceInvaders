#pragma once
#include "Sys.h"
#include "Direction.h"
#include <random>

class SysManager;

class Sys_BunkerControl : public Sys
{
public:
	Sys_BunkerControl(SysManager* systemManager);
	~Sys_BunkerControl();
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
	void damageBunker(const ActorId& actorId, const ActorId& otherId);
	bool nonTransparentPixels(unsigned int xStart, unsigned int xEnd, unsigned int yEnd, unsigned int height, sf::Image& image);
	void turnOffPixels(unsigned int xStart, unsigned int xEnd, unsigned int yStart, unsigned int yEnd, sf::Image& image);

	sf::Texture m_damageTexture;
	sf::Sprite m_damageSprite;

	int m_rows, m_cols;

	std::random_device m_rd;
	std::mt19937 m_gen;
	std::uniform_real_distribution<float> m_unifAngleDist; // Uniform(0, 360) distribution
	std::uniform_real_distribution<float> m_unifLengthDist; // Uniform(0, 10) distribution

};