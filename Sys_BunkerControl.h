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
	void damageBunker(const ActorId& actorId, const ActorId& otherId);
	void applyDamageToBunker(sf::Sprite* bunkerSprite, sf::Texture* texture, const sf::FloatRect& spriteRect, const sf::FloatRect& intersect, sf::Image& image);
	bool pixelCollision(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image);
	void turnOffPixels(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image);
	sf::Vector2i convertToPixelCoords(const sf::Vector2f& globalCoords, const sf::Sprite* sprite);
	sf::Vector2f convertToScreenCoords(const sf::Vector2i& pixelCoords, const sf::Sprite* sprite);

	void drawCells(sf::RenderWindow* window, Grid* grid);
	void drawGrids(WindowManager* windowManager);

	sf::Texture m_damageTexture;
	sf::Sprite m_damageSprite;

	int m_rows, m_cols;
	std::random_device m_rd;
	std::mt19937 m_randGen;
#ifdef DEBUG
	std::vector<sf::RectangleShape> m_bulletsIntersects;
	std::vector<sf::RectangleShape> m_bulletTips;
	std::vector<sf::RectangleShape> m_disabledCells;
#endif
};