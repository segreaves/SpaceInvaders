#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

class ActorManager;
class Comp_Collision;
class Comp_SpriteSheet;

using ActorId = unsigned int;

enum class LevelState { PlayerAlive = 0, PlayerDestroyed, PlayerInvaded };

/// <summary>
/// Sets up the spawn points for the player, invaders, and bunkers.
/// Provides the speed of invaders to the game state as the levels progress.
/// </summary>
class LevelManager
{
public:
	LevelManager(ActorManager* actorManager);
	~LevelManager();

	sf::Vector2f getPlayerSpawnPoint() const;

	sf::FloatRect getViewSpace() const { return m_viewSpace; }
	void setViewSpace(sf::FloatRect viewSpace) { m_viewSpace = viewSpace; }
	sf::Vector2f getScreenCenter() { return sf::Vector2f(m_viewSpace.left + m_viewSpace.width / 2.f, m_viewSpace.top + m_viewSpace.height / 2.f); }
	void createPlayer();
	void createInvaders();
	void createPlayerBullets();
	void createInvaderBullets();
	void createBunkers();
	void createShockwaves();
	void createPlayerExplosion();
	ActorId& getPlayerId() { return m_playerId; }
	std::vector<ActorId>& getInvaderIds() { return m_invaders; }
	std::vector<ActorId>& getPlayerBulletIds() { return m_playerBullets; }
	std::vector<ActorId>& getInvaderBulletIds() { return m_invaderBullets; }
	std::vector<ActorId>& getBunkerIds() { return m_bunkers; }
	std::vector<ActorId>& getShockwaveIds() { return m_shockwaves; }
	ActorId& getPlayerExplosionId() { return m_playerExplosion; }
	int getPlayerLives() const;
	void setPlayerLives(const unsigned int lives) { m_playerLives = lives; }
	int getInvaderCount() const { return m_remainingInvaders; }
	void resetInvaderCount() { m_remainingInvaders = static_cast<int>(m_invaders.size()); }
	void onInvaderDefeated();
	void onPlayerDefeated();
	int getKills() const { return m_kills; }
	sf::Vector2f getBunkerSpawn(ActorId id);
	unsigned int getLevel() const { return m_level; }
	LevelState getState() const { return m_state; }
	void setState(const LevelState state) { m_state = state; }
	float getInvaderBaseSpeed() const { return m_invaderBaseSpeed; }
	float getLevelSpeedIncrease() const { return m_levelSpeedIncrease; }
	float getKillSpeedIncrease() const { return m_defeatSpeedIncrease; }
	unsigned int getScore() { return m_score; }
	void resetKillStreak();
	void newGame();
	void purge();

	// overload the ++ operator
	unsigned int operator++() { return ++m_level; }
	unsigned int operator++(int) { return m_level++; }
private:
	void adjustColliderToSprite(std::shared_ptr<Comp_Collision> colComp, std::shared_ptr<Comp_SpriteSheet> sprite);

	unsigned int m_level;
	LevelState m_state;
	ActorManager* m_actorManager;
	ActorId m_playerId;
	std::vector<ActorId> m_invaders;
	std::vector<ActorId> m_playerBullets;
	std::vector<ActorId> m_invaderBullets;
	std::vector<ActorId> m_bunkers;
	std::vector<ActorId> m_shockwaves;
	ActorId m_playerExplosion;
	unsigned int m_remainingInvaders;
	unsigned int m_playerLives;
	unsigned int m_kills;
	unsigned int m_score;
	unsigned int m_invaderBasePoints = 1;
	unsigned int m_invaderPoints = 1;
	bool m_killStreak = false;
	const int m_invaderCols = 12;
	const sf::Vector2f m_invaderSeparation = sf::Vector2f(50, 45);
	const int m_nBullets = 100;
	const int m_nBunkers = 5;
	const float m_bunkerSeparation = 170;
	const float m_bunkerSpawnHeight = 160;
	std::unordered_map<ActorId, sf::Vector2f> m_bunkerSpawn;
	sf::FloatRect m_viewSpace;
	const float m_invaderBaseSpeed = 100;
	const float m_levelSpeedIncrease = 10;
	const float m_defeatSpeedIncrease = 3;
};