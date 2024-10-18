#include "LevelManager.h"
#include "Context.h"
#include "Utils.h"
#include <fstream>
#include <sstream>

LevelManager::LevelManager() :
	m_actorManager(nullptr),
	m_level(0),
	m_playerId(-1),
	m_playerExplosion(-1),
	m_playerLives(0),
	m_remainingInvaders(0),
	m_kills(0)
{
}

LevelManager::~LevelManager()
{
}

sf::Vector2f LevelManager::getPlayerSpawnPoint() const
{
	return sf::Vector2f(m_viewSpace.left + m_viewSpace.width / 2.f, m_viewSpace.top + m_viewSpace.height * 0.925f);
}

void LevelManager::setViewSpace(sf::FloatRect viewSpace)
{
	m_viewSpace = viewSpace;
}

void LevelManager::createPlayer()
{
	m_playerId = m_actorManager->loadActorProfile("player", "player");
	Comp_Position* posComp = m_actorManager->getActor(m_playerId)->getComponent<Comp_Position>(ComponentType::Position);
	posComp->setPosition(getPlayerSpawnPoint());
	// get collider and adjust to sprite
	Comp_Collision* colComp = m_actorManager->getActor(m_playerId)->getComponent<Comp_Collision>(ComponentType::Collision);
	Comp_SpriteSheet* sprite = m_actorManager->getActor(m_playerId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	adjustColliderToSprite(colComp, sprite);
}

void LevelManager::createInvaders(sf::FloatRect viewSpace)
{
	float offset = (viewSpace.width - m_invaderSeparation.x * m_invaderCols) / 2.f;
	std::vector<std::string> invaderProfiles = { "invader1", "invader2", "invader3", "invader2", "invader1" };
	for (int i = 0; i < invaderProfiles.size(); i++)
	{
		for (int j = 0; j < m_invaderCols; j++)
		{
			int invaderId = m_actorManager->loadActorProfile(invaderProfiles[i], "invader");
			m_invaders.emplace_back(invaderId);
			float spawnX = j * m_invaderSeparation.x + offset;// center in view space
			float spawnY = (i + 1) * m_invaderSeparation.y;
			Comp_Invader* invaderComp = m_actorManager->getActor(invaderId)->getComponent<Comp_Invader>(ComponentType::Invader);
			invaderComp->setSpawnPosition(sf::Vector2f(spawnX, spawnY));
			// adjust collider to fit sprite
			Comp_Collision* colComp = m_actorManager->getActor(invaderId)->getComponent<Comp_Collision>(ComponentType::Collision);
			Comp_SpriteSheet* sprite = m_actorManager->getActor(invaderId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
			adjustColliderToSprite(colComp, sprite);
		}
	}
}

void LevelManager::createPlayerBullets()
{
	for (unsigned int i = 0; i < m_nBullets; i++)
		m_playerBullets.push_back(m_actorManager->loadActorProfile("bullet_player", "bullet_player"));
}

void LevelManager::createInvaderBullets()
{
	for (unsigned int i = 0; i < m_nBullets; i++)
		m_invaderBullets.push_back(m_actorManager->loadActorProfile("bullet_invader", "bullet_invader"));
}

void LevelManager::createBunkers(sf::FloatRect viewSpace)
{
	float offset = (viewSpace.width - m_bunkerSeparation * (m_nBunkers - 1)) / 2.f;
	float spawnY = viewSpace.height - m_bunkerSpawnHeight;
	for (int i = 0; i < m_nBunkers; i++)
	{
		int bunkerId = m_actorManager->loadActorProfile("bunker", "bunker");
		m_bunkers.emplace_back(bunkerId);
		float spawnX = i * m_bunkerSeparation + offset;// center in view space
		m_bunkerSpawn.emplace(bunkerId, sf::Vector2f(spawnX, spawnY));
		// get collider and adjust to sprite
		Comp_Collision* colComp = m_actorManager->getActor(bunkerId)->getComponent<Comp_Collision>(ComponentType::Collision);
		Comp_SpriteSheet* sprite = m_actorManager->getActor(bunkerId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		adjustColliderToSprite(colComp, sprite);
		m_actorManager->enableActor(bunkerId);
		Comp_Position* posComp = m_actorManager->getActor(bunkerId)->getComponent<Comp_Position>(ComponentType::Position);
		posComp->setPosition(getBunkerSpawn(bunkerId));
		// fit bunker grids to sprite
		Comp_Grid* gridComp = m_actorManager->getActor(bunkerId)->getComponent<Comp_Grid>(ComponentType::Grid);
		// get position of the sprite's top left corner
		sf::Vector2f gridPos(posComp->getPosition().x + sprite->getDrawableBounds().left, posComp->getPosition().y + sprite->getDrawableBounds().top);
		gridComp->create(sprite->getDrawableBounds().width, sprite->getDrawableBounds().height, gridPos);
	}
}

void LevelManager::createShockwaves(const int& numInvaders)
{
	for (unsigned int i = 0; i < numInvaders; i++)
	{
		unsigned int shockwaveId = m_actorManager->loadActorProfile("shockwave", "shockwave");
		m_shockwaves.push_back(shockwaveId);
		Comp_SpriteSheet* sprite = m_actorManager->getActor(shockwaveId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		sf::Color shockwaveColor = APP_COLOR;
		shockwaveColor.a = 50;
		sprite->getSpriteSheet()->setSpriteColor(shockwaveColor);
	}
}

void LevelManager::createPlayerExplosion()
{
	m_playerExplosion = m_actorManager->loadActorProfile("player_explosion", "player_explosion");
}

int LevelManager::getPlayerLives() const
{
	Comp_Health* healthComp = m_actorManager->getActor(m_playerId)->getComponent<Comp_Health>(ComponentType::Health);
	if (!healthComp) return 0;
	return healthComp->getHealth();
}

void LevelManager::onInvaderDefeated()
{
	m_kills++;
	decrementInvaderCount();
}

sf::Vector2f LevelManager::getBunkerSpawn(ActorId id)
{
	if (m_bunkerSpawn.find(id) == m_bunkerSpawn.end()) return sf::Vector2f();
	return m_bunkerSpawn[id];
}

void LevelManager::adjustColliderToSprite(Comp_Collision* colComp, Comp_SpriteSheet* sprite)
{
	sf::Vector2f size = sf::Vector2f(
		sprite->getSpriteSheet()->getSpriteScale().x * sprite->getSpriteSheet()->getSpriteSize().x,
		sprite->getSpriteSheet()->getSpriteScale().y * sprite->getSpriteSheet()->getSpriteSize().y
		);
	colComp->setAABB(size);
}
