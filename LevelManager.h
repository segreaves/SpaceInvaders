#pragma once
#include <SFML/Graphics.hpp>

class Context;

using ActorId = unsigned int;

/// <summary>
/// Sets up the spawn points for the player and invaders.
/// Increases the speed of invaders as the levels progress.
/// </summary>
class LevelManager
{
public:
	LevelManager();
	~LevelManager();

	void loadActorProfile(const std::string actorName);

	std::vector<sf::Vector2f> getInvaderSpawnPoints();
	void setInvaderSpawnPoints(const sf::Vector2f& invaderSize);
	std::vector<sf::Vector2f> getBunkerSpawnPoints();
	void setBunkerSpawnPoints();
	sf::Vector2f getPlayerSpawnPoint() const;
	unsigned int getPlayerId() const { return m_playerId; }
	std::vector<ActorId> getInvaderIds() const { return m_invaders; }
	std::vector<ActorId> getBulletIds() const { return m_bullets; }
	std::vector<ActorId> getBunkerIds() const { return m_bunkers; }

	void setViewSpace(sf::FloatRect viewSpace);
	sf::FloatRect getViewSpace() const { return m_viewSpace; }
	int getTotalInvaders() const { return m_invaderRows * m_invaderCols; }
	int getTotalBunkers() const { return m_nBunkers; }
	sf::Vector2f getBunkerSize() const { return m_bunkerSize; }
	float getInvaderStartSpeed() const { return m_invaderStartSpeed; }
	float getLevelSpeedIncrease() const { return m_levelSpeedIncrease; }
	float getDefeatSpeedIncrease() const { return m_defeatSpeedIncrease; }
	void setContext(Context* context) { m_context = context; }

	unsigned int m_level;
private:
	std::vector<sf::Vector2f> getGridFormation(unsigned int rows, unsigned int cols, float deltaX, float deltaY, float padding);

	Context* m_context;
	int m_playerId;
	std::vector<ActorId> m_invaders;
	std::vector<ActorId> m_bullets;
	std::vector<ActorId> m_bunkers;
	std::vector<sf::Vector2f> m_invaderSpawnPoints;
	std::vector<sf::Vector2f> m_bunkerSpawnPoints; const int m_nBunkers = 6;
	const sf::Vector2f m_bunkerSize = sf::Vector2f(60, 55);
	sf::FloatRect m_viewSpace;
	const unsigned int m_invaderRows = 5;
	const unsigned int m_invaderCols = 12;
	const float m_invaderPadding = 10.f;
	const float m_invaderStartSpeed = 100;
	const float m_levelSpeedIncrease = 10;
	const float m_defeatSpeedIncrease = 2;
};