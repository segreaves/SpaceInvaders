#pragma once
#include <SFML/Graphics.hpp>
#include "Trigger.h"
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

	Trigger<unsigned int> m_updateScore;

	sf::Vector2f getPlayerSpawnPoint() const;

	sf::FloatRect getViewSpace() const { return m_viewSpace; }
	void setViewSpace(sf::FloatRect viewSpace) { m_viewSpace = viewSpace; }
	sf::Vector2f getScreenCenter() { return sf::Vector2f(m_viewSpace.left + m_viewSpace.width / 2.f, m_viewSpace.top + m_viewSpace.height / 2.f); }
	void createPlayer();
	void createInvaders();
	void createPlayerBullets();
	void createInvaderBullets();
	void createBunkers();
	void createUFO();
	void createShockwaves();
	void createPlayerExplosion();
	void createUFOExplosion();
	int getTotalInvaders() const { return m_invaderCols * static_cast<int>(m_invaderProfiles.size()); }
	ActorId& getPlayerId() { return m_playerId; }
	std::vector<ActorId>& getInvaderIds() { return m_invaders; }
	std::vector<ActorId>& getPlayerBulletIds() { return m_playerBullets; }
	std::vector<ActorId>& getInvaderBulletIds() { return m_invaderBullets; }
	std::vector<ActorId>& getBunkerIds() { return m_bunkers; }
	std::vector<ActorId>& getShockwaveIds() { return m_shockwaves; }
	ActorId& getPlayerExplosionId() { return m_playerExplosion; }
	ActorId& getUFOExplosionId() { return m_ufoExplosion; }
	ActorId& getUFOId() { return m_ufo; }
	int getPlayerLives() const;
	void setPlayerLives(const unsigned int lives) { m_playerLives = lives; }
	int getInvaderCount() const { return m_remainingInvaders; }
	void resetInvaderCount();
	void onInvaderDefeated();
	void onUFODefeated();
	void onPlayerDefeated();
	int getKills() const { return m_kills; }
	sf::Vector2f getBunkerSpawn(ActorId id);
	unsigned int getLevel() const { return m_level; }
	LevelState getState() const { return m_state; }
	void setState(const LevelState state) { m_state = state; }
	float getInvaderBaseSpeed() const { return m_invaderBaseSpeed; }
	float getLevelSpeedIncrease() const { return m_levelSpeedIncrease; }
	float getKillSpeedIncrease() const { return m_killSpeedIncrease; }
	float getLevelBaseSpeed() const { return m_invaderBaseSpeed + (m_level - 1) * m_levelSpeedIncrease; }
	unsigned int getScore() { return m_score; }
	void resetKillStreak();
	void newGame();
	void purge();

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
	ActorId m_ufo;
	ActorId m_playerExplosion;
	ActorId m_ufoExplosion;
	unsigned int m_remainingInvaders;
	unsigned int m_playerLives;
	unsigned int m_kills;
	unsigned int m_score;
	unsigned int m_points;
	unsigned int m_ufoPoints = 1000;
	bool m_killStreak = false;
	const int m_invaderCols = 12;
	const std::vector<std::string> m_invaderProfiles = { "invader1", "invader2", "invader3", "invader2", "invader1" };
	const sf::Vector2f m_invaderSeparation = sf::Vector2f(50, 50);
	const int m_nBullets = 100;
	const int m_nBunkers = 5;
	const float m_bunkerSeparation = 170;
	const float m_bunkerSpawnHeight = 160;
	std::unordered_map<ActorId, sf::Vector2f> m_bunkerSpawn;
	sf::FloatRect m_viewSpace;
	const float m_invaderBaseSpeed = 50;
	const float m_levelSpeedIncrease = 20;
	const float m_killSpeedIncrease = 4;
};