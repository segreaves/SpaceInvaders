#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

class Context;

/// <summary>
/// Sets up the spawn points for the player and invaders.
/// </summary>
class Level
{
public:
	Level(Context* context);
	~Level();

	std::vector<sf::Vector2f> getInvaderSpawnPoints(sf::FloatRect viewSpace, const sf::Vector2f& invaderSize);
	sf::Vector2f getPlayerSpawnPoint(sf::FloatRect viewSpace) const;
private:
	std::vector<sf::Vector2f> getGridFormation(unsigned int rows, unsigned int columns, float lengthX, float lengthY);

	Context* m_context;
	const unsigned int m_numInvaders = 50;
};