#pragma once
#include "Sys.h"

class SysManager;

class Sys_BunkerDamage : public Sys
{
	public:
	Sys_BunkerDamage(SysManager* systemManager);
	~Sys_BunkerDamage();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

private:
	void handleBunkerDamage(const ActorId& actorId, const ActorId& otherId);
	sf::Vector2i pixelCollision(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image);
	void turnOffPixels(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image);
};