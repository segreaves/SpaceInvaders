#pragma once
#include "Sys.h"
#include "Direction.h"

class SysManager;

class Sys_BunkerControl : public Sys
{
	public:
	Sys_BunkerControl(SysManager* systemManager);
	~Sys_BunkerControl();

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
	bool pixelCollision(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image);
	sf::Vector2i convertToPixelCoords(const sf::Vector2f& coords, const sf::Sprite* sprite);
	sf::Vector2f convertToScreenCoords(const sf::Vector2i& coords, const sf::Sprite* sprite);

	sf::Texture m_damageTexture;
	sf::Sprite m_damageSprite;
};