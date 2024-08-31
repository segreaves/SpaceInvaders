#pragma once
#include <SFML/Graphics.hpp>

class Context;

using TileId = unsigned int;

struct Tile
{
	Tile() :
		m_id(0),
		m_enabled(true)
	{
		m_shape.setSize(sf::Vector2f(1, 1));
		m_shape.setFillColor(sf::Color::White);
	}

	Tile(int id, float tileSize) :
		m_enabled(true)
	{
		m_id = id;
		m_shape.setSize(sf::Vector2f(tileSize, tileSize));
		m_shape.setFillColor(sf::Color::White);
	}

	sf::RectangleShape m_shape;
	TileId m_id;
	bool m_enabled;
};

/// <summary>
/// Sets up the spawn points for the player and invaders.
/// Increases the speed of invaders as the levels progress.
/// </summary>
class LevelManager
{
public:
	LevelManager();
	~LevelManager();

	std::vector<sf::Vector2f> getInvaderSpawnPoints();
	void setInvaderSpawnPoints(const sf::Vector2f& invaderSize);
	std::vector<sf::Vector2f> getBunkerSpawnPoints();
	void setBunkerSpawnPoints();
	sf::Vector2f getPlayerSpawnPoint() const;

	void setViewSpace(sf::FloatRect viewSpace);
	sf::FloatRect getViewSpace() const { return m_viewSpace; }
	int getTotalInvaders() const { return m_invaderRows * m_invaderCols; }
	int getTotalBunkers() const { return m_nBunkers; }
	sf::Vector2f getBunkerSize() const { return m_bunkerSize; }
	float getInvaderStartSpeed() const { return m_invaderStartSpeed; }
	float getLevelSpeedIncrease() const { return m_levelSpeedIncrease; }
	float getDefeatSpeedIncrease() const { return m_defeatSpeedIncrease; }

	unsigned int m_level;
private:
	void loadProfiles(const std::string& path);
	std::vector<sf::Vector2f> getGridFormation(unsigned int rows, unsigned int cols, float deltaX, float deltaY, float padding);

	std::vector<sf::Vector2f> m_invaderSpawnPoints;
	std::vector<sf::Vector2f> m_bunkerSpawnPoints; const int m_nBunkers = 6;
	const sf::Vector2f m_bunkerSize = sf::Vector2f(60, 55);
	sf::FloatRect m_viewSpace;
	const unsigned int m_invaderRows = 2;
	const unsigned int m_invaderCols = 2;
	const float m_invaderPadding = 10.f;
	const float m_invaderStartSpeed = 100;
	const float m_levelSpeedIncrease = 10;
	const float m_defeatSpeedIncrease = 10;
	
	std::string m_playerPath;
	std::string m_invaderPath;
	std::string m_bulletPath;
	std::string m_bunkerPath;
};