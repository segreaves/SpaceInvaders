#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

class ActorManager;

using ActorId = unsigned int;

/// <summary>
/// Sets up the spawn points for the player, invaders, and bunkers.
/// Provides the speed of invaders to the game state as the levels progress.
/// </summary>
class LevelManager
{
public:
	LevelManager();
	~LevelManager();

	int loadActorProfile(const std::string actorName);

	std::vector<sf::Vector2f> getBunkerSpawnPoints();
	void setBunkerSpawnPoints();
	sf::Vector2f getPlayerSpawnPoint() const;


	void setViewSpace(sf::FloatRect viewSpace);
	sf::FloatRect getViewSpace() const { return m_viewSpace; }
	void createInvaders(sf::FloatRect viewSpace);
	std::vector<ActorId>& getInvaderIds() { return m_invaders; }
	sf::Vector2f getInvaderSpawn(ActorId id);
	int getTotalBunkers() const { return m_nBunkers; }
	sf::Vector2f getBunkerSize() const { return m_bunkerSize; }
	float getInvaderStartSpeed() const { return m_invaderStartSpeed; }
	float getLevelSpeedIncrease() const { return m_levelSpeedIncrease; }
	float getDefeatSpeedIncrease() const { return m_defeatSpeedIncrease; }
	void setActorManager(ActorManager* actorManager) { m_actorManager = actorManager; }

	unsigned int m_level;
private:
	std::vector<sf::Vector2f> getGridFormation(unsigned int rows, unsigned int cols, float deltaX, float deltaY, float padding);

	ActorManager* m_actorManager;
	std::vector<ActorId> m_invaders;
	const sf::Vector2i m_invaderGridDims = sf::Vector2i(5, 12);
	const sf::Vector2i m_invaderSeparation = sf::Vector2i(55, 50);
	std::unordered_map<ActorId, sf::Vector2f> m_invaderSpawn;
	std::vector<sf::Vector2f> m_bunkerSpawnPoints;
	const int m_nBunkers = 6;
	const sf::Vector2f m_bunkerSize = sf::Vector2f(60, 55);
	sf::FloatRect m_viewSpace;
	const float m_invaderPadding = 10.f;
	const float m_invaderStartSpeed = 100;
	const float m_levelSpeedIncrease = 10;
	const float m_defeatSpeedIncrease = 2;
};