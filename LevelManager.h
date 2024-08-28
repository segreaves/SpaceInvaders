#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

class Context;

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
	sf::Vector2f getPlayerSpawnPoint() const;

	void setViewSpace(sf::FloatRect viewSpace);
	sf::FloatRect getViewSpace() const { return m_viewSpace; }
	int getTotalInvaders() const { return m_invaderRows * m_invaderCols; }
	float getInvaderStartSpeed() const { return m_invaderStartSpeed; }
	float getSpeedIncrease() const { return m_speedIncrease; }

	unsigned int m_level;
private:
	void loadProfiles(const std::string& path);
	std::vector<sf::Vector2f> getGridFormation(unsigned int rows, unsigned int cols, float invaderSizeX, float invaderSizeY, float padding);

	std::vector<sf::Vector2f> m_invaderSpawnPoints;
	sf::FloatRect m_viewSpace;
	const unsigned int m_invaderRows = 2;
	const unsigned int m_invaderCols = 2;
	const float m_invaderPadding = 10.f;
	std::string m_playerPath;
	std::string m_invaderPath;
	std::string m_bulletPath;
	const float m_invaderStartSpeed = 100;
	const float m_speedIncrease = 10;
};