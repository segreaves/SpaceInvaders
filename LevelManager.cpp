#include "LevelManager.h"
#include "Context.h"
#include "Utils.h"
#include <fstream>
#include <sstream>

LevelManager::LevelManager() :
	m_actorManager(nullptr),
	m_level(0),
	m_playerId(-1)
{
}

LevelManager::~LevelManager()
{
}

sf::Vector2f LevelManager::getPlayerSpawnPoint() const
{
	return sf::Vector2f(m_viewSpace.left + m_viewSpace.width / 2.f, m_viewSpace.top + m_viewSpace.height * 0.9f);
}

void LevelManager::setViewSpace(sf::FloatRect viewSpace)
{
	m_viewSpace = viewSpace;
}

void LevelManager::createPlayer()
{
	m_playerId = m_actorManager->loadActorProfile("player");
	Comp_Position* posComp = m_actorManager->getActor(m_playerId)->getComponent<Comp_Position>(ComponentType::Position);
	posComp->setPosition(getPlayerSpawnPoint());
}

void LevelManager::createInvaders(sf::FloatRect viewSpace)
{
	float offset = (viewSpace.width - m_invaderSeparation.x * m_invaderCols) / 2.f;
	for (int i = 0; i < m_invaderRows; i++)
	{
		for (int j = 0; j < m_invaderCols; j++)
		{
			int invaderId = m_actorManager->loadActorProfile("invader");
			m_invaders.emplace_back(invaderId);
			float spawnX = j * m_invaderSeparation.x + offset;// center in view space
			float spawnY = (i + 1) * m_invaderSeparation.y;
			m_invaderSpawn.emplace(invaderId, sf::Vector2f(spawnX, spawnY));
		}
	}
}

void LevelManager::createPlayerBullets()
{
	for (unsigned int i = 0; i < m_nBullets; i++)
		m_playerBullets.push_back(m_actorManager->loadActorProfile("bullet_player"));
}

void LevelManager::createInvaderBullets()
{
	for (unsigned int i = 0; i < m_nBullets; i++)
		m_invaderBullets.push_back(m_actorManager->loadActorProfile("bullet_invader"));
}

void LevelManager::createBunkers(sf::FloatRect viewSpace)
{
	float offset = (viewSpace.width - m_bunkerSeparation * (m_nBunkers - 1)) / 2.f;
	float spawnY = viewSpace.height - m_bunkerSpawnHeight;
	for (int i = 0; i < m_nBunkers; i++)
	{
		int bunkerId = m_actorManager->loadActorProfile("bunker");
		m_bunkers.emplace_back(bunkerId);
		float spawnX = i * m_bunkerSeparation + offset;// center in view space
		m_bunkerSpawn.emplace(bunkerId, sf::Vector2f(spawnX, spawnY));
	}
}

void LevelManager::createShockwaves()
{
	for (unsigned int i = 0; i < m_nBullets; i++)
		m_shockwaves.push_back(m_actorManager->loadActorProfile("shockwave"));
}

sf::Vector2f LevelManager::getInvaderSpawn(ActorId id)
{
	if (m_invaderSpawn.find(id) == m_invaderSpawn.end()) return sf::Vector2f();
	return m_invaderSpawn[id];
}

sf::Vector2f LevelManager::getBunkerSpawn(ActorId id)
{
	if (m_bunkerSpawn.find(id) == m_bunkerSpawn.end()) return sf::Vector2f();
	return m_bunkerSpawn[id];
}

std::vector<sf::Vector2f> LevelManager::getGridFormation(unsigned int rows, unsigned int cols, float deltaX, float deltaY, float padding)
{
	std::vector<sf::Vector2f> formationPoints;
	formationPoints.reserve(rows * cols);
	for (int row = 0; row < rows; row++)
	{
		for (int col = 0; col < cols; col++)
		{
			float x = (deltaX + padding) * col;
			float y = (deltaY + padding) * row;
			formationPoints.emplace_back(sf::Vector2f(x, y));
		}
	}
	return formationPoints;
}
