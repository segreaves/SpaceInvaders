#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

class ActorManager;
class Comp_Collision;
class Comp_SpriteSheet;

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

	sf::Vector2f getPlayerSpawnPoint() const;

	void setViewSpace(sf::FloatRect viewSpace);
	sf::FloatRect getViewSpace() const { return m_viewSpace; }
	void createPlayer();
	void createInvaders(sf::FloatRect viewSpace);
	void createPlayerBullets();
	void createInvaderBullets();
	void createBunkers(sf::FloatRect viewSpace);
	void createShockwaves(const int& numInvaders);
	int& getPlayerId() { return m_playerId; }
	std::vector<ActorId>& getInvaderIds() { return m_invaders; }
	std::vector<ActorId>& getPlayerBulletIds() { return m_playerBullets; }
	std::vector<ActorId>& getInvaderBulletIds() { return m_invaderBullets; }
	std::vector<ActorId>& getBunkerIds() { return m_bunkers; }
	std::vector<ActorId>& getShockwaveIds() { return m_shockwaves; }
	sf::Vector2f getInvaderSpawn(ActorId id);
	sf::Vector2f getBunkerSpawn(ActorId id);
	unsigned int getLevel() const { return m_level; }
	float getInvaderBaseSpeed() const { return m_invaderBaseSpeed; }
	float getLevelSpeedIncrease() const { return m_levelSpeedIncrease; }
	float getDefeatSpeedIncrease() const { return m_defeatSpeedIncrease; }
	void setActorManager(ActorManager* actorManager) { m_actorManager = actorManager; }

	// overload the ++ operator
	unsigned int operator++() { return ++m_level; }
	unsigned int operator++(int) { return m_level++; }
private:
	void adjustColliderToSprite(Comp_Collision* colComp, Comp_SpriteSheet* sprite);

	unsigned int m_level;
	ActorManager* m_actorManager;
	int m_playerId;
	std::vector<ActorId> m_invaders;
	std::vector<ActorId> m_playerBullets;
	std::vector<ActorId> m_invaderBullets;
	std::vector<ActorId> m_bunkers;
	std::vector<ActorId> m_shockwaves;
	std::unordered_map<ActorId, sf::Vector2f> m_invaderSpawn;
	const int m_invaderCols = 12;
	const sf::Vector2i m_invaderSeparation = sf::Vector2i(50, 45);
	const int m_nBullets = 100;
	const int m_nBunkers = 5;
	const float m_bunkerSeparation = 170;
	const float m_bunkerSpawnHeight = 160;
	std::unordered_map<ActorId, sf::Vector2f> m_bunkerSpawn;
	sf::FloatRect m_viewSpace;
	const float m_invaderPadding = 10.f;
	const float m_invaderBaseSpeed = 100;
	const float m_levelSpeedIncrease = 10;
	const float m_defeatSpeedIncrease = 3;
};