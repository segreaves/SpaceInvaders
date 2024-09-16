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

	sf::Vector2f getPlayerSpawnPoint() const;

	void setViewSpace(sf::FloatRect viewSpace);
	sf::FloatRect getViewSpace() const { return m_viewSpace; }
	void createPlayer();
	void createInvaders(sf::FloatRect viewSpace);
	void createBullets();
	void createBunkers(sf::FloatRect viewSpace);
	void createShockwaves();
	int& getPlayerId() { return m_playerId; }
	std::vector<ActorId>& getInvaderIds() { return m_invaders; }
	std::vector<ActorId>& getBulletIds() { return m_bullets; }
	std::vector<ActorId>& getBunkerIds() { return m_bunkers; }
	std::vector<ActorId>& getShockwaveIds() { return m_shockwaves; }
	sf::Vector2f getInvaderSpawn(ActorId id);
	sf::Vector2f getBunkerSpawn(ActorId id);
	float getInvaderStartSpeed() const { return m_invaderStartSpeed; }
	float getLevelSpeedIncrease() const { return m_levelSpeedIncrease; }
	float getDefeatSpeedIncrease() const { return m_defeatSpeedIncrease; }
	void setActorManager(ActorManager* actorManager) { m_actorManager = actorManager; }

	unsigned int m_level;
private:
	std::vector<sf::Vector2f> getGridFormation(unsigned int rows, unsigned int cols, float deltaX, float deltaY, float padding);

	ActorManager* m_actorManager;
	int m_playerId;
	std::vector<ActorId> m_invaders;
	std::vector<ActorId> m_bullets;
	std::vector<ActorId> m_bunkers;
	std::vector<ActorId> m_shockwaves;
	std::unordered_map<ActorId, sf::Vector2f> m_invaderSpawn;
	const int m_invaderRows = 5;
	const int m_invaderCols = 12;
	const sf::Vector2i m_invaderSeparation = sf::Vector2i(55, 50);
	const int m_nBullets = 100;
	const int m_nBunkers = 5;
	const float m_bunkerSeparation = 170;
	const float m_bunkerSpawnHeight = 250;
	std::unordered_map<ActorId, sf::Vector2f> m_bunkerSpawn;
	const sf::Vector2f m_bunkerSize = sf::Vector2f(60, 55);
	sf::FloatRect m_viewSpace;
	const float m_invaderPadding = 10.f;
	const float m_invaderStartSpeed = 100;
	const float m_levelSpeedIncrease = 10;
	const float m_defeatSpeedIncrease = 2;
};