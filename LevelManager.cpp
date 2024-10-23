#include "LevelManager.h"
#include "Context.h"
#include "Utils.h"
#include <fstream>
#include <sstream>

LevelManager::LevelManager(ActorManager* actorManager) :
	m_actorManager(actorManager),
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

void LevelManager::createPlayer()
{
	m_playerId = m_actorManager->loadActorProfile("player", "player");
	auto posComp = m_actorManager->getActor(m_playerId)->getComponent<Comp_Position>(ComponentType::Position);
	posComp->setPosition(getPlayerSpawnPoint());
	// get collider and adjust to sprite
	auto colComp = m_actorManager->getActor(m_playerId)->getComponent<Comp_Collision>(ComponentType::Collision);
	auto sprite = m_actorManager->getActor(m_playerId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	adjustColliderToSprite(colComp, sprite);
}

void LevelManager::createInvaders(sf::FloatRect viewSpace)
{
	const auto offset = (viewSpace.width - m_invaderSeparation.x * m_invaderCols) / 2.f;
	std::vector<std::string> invaderProfiles = { "invader1", "invader2", "invader3", "invader2", "invader1" };
	for (auto i = 0; i < invaderProfiles.size(); i++)
	{
		for (auto j = 0; j < m_invaderCols; j++)
		{
			const auto invaderId = m_actorManager->loadActorProfile(invaderProfiles[i], "invader");
			m_invaders.emplace_back(invaderId);
			const auto spawnPos = sf::Vector2f(
				j * m_invaderSeparation.x + offset,
				(i + 1) * m_invaderSeparation.y);
			auto invaderComp = m_actorManager->getActor(invaderId)->getComponent<Comp_Invader>(ComponentType::Invader);
			invaderComp->setSpawnPosition(spawnPos);
			// adjust collider to fit sprite
			auto colComp = m_actorManager->getActor(invaderId)->getComponent<Comp_Collision>(ComponentType::Collision);
			auto sprite = m_actorManager->getActor(invaderId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
			adjustColliderToSprite(colComp, sprite);
		}
	}
}

void LevelManager::createPlayerBullets()
{
	for (auto i = 0; i < m_nBullets; i++)
		m_playerBullets.emplace_back(m_actorManager->loadActorProfile("bullet_player", "bullet_player"));
}

void LevelManager::createInvaderBullets()
{
	for (auto i = 0; i < m_nBullets; i++)
		m_invaderBullets.emplace_back(m_actorManager->loadActorProfile("bullet_invader", "bullet_invader"));
}

void LevelManager::createBunkers(sf::FloatRect viewSpace)
{
	const auto offset = (viewSpace.width - m_bunkerSeparation * (m_nBunkers - 1)) / 2.f;
	for (auto i = 0; i < m_nBunkers; i++)
	{
		const auto bunkerId = m_actorManager->loadActorProfile("bunker", "bunker");
		m_bunkers.emplace_back(bunkerId);
		m_bunkerSpawn.emplace(bunkerId, sf::Vector2f(i * m_bunkerSeparation + offset, viewSpace.height - m_bunkerSpawnHeight));
		// get collider and adjust to sprite
		auto colComp = m_actorManager->getActor(bunkerId)->getComponent<Comp_Collision>(ComponentType::Collision);
		auto sprite = m_actorManager->getActor(bunkerId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		adjustColliderToSprite(colComp, sprite);
		m_actorManager->enableActor(bunkerId);
		auto posComp = m_actorManager->getActor(bunkerId)->getComponent<Comp_Position>(ComponentType::Position);
		posComp->setPosition(getBunkerSpawn(bunkerId));
		// fit bunker grids to sprite
		auto gridComp = m_actorManager->getActor(bunkerId)->getComponent<Comp_Grid>(ComponentType::Grid);
		// get position of the sprite's top left corner
		sf::Vector2f gridPos(posComp->getPosition().x + sprite->getDrawableBounds().left, posComp->getPosition().y + sprite->getDrawableBounds().top);
		gridComp->create(sprite->getDrawableBounds().width, sprite->getDrawableBounds().height, gridPos);
	}
}

void LevelManager::createShockwaves()
{
	for (auto i = 0; i < m_invaders.size(); i++)
	{
		const auto shockwaveId = m_actorManager->loadActorProfile("shockwave", "shockwave");
		m_shockwaves.emplace_back(shockwaveId);
		auto sprite = m_actorManager->getActor(shockwaveId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		const auto shockwaveColor = APP_COLOR_TRANSP;
		sprite->getSpriteSheet()->setSpriteColor(shockwaveColor);
	}
}

void LevelManager::createPlayerExplosion()
{
	m_playerExplosion = m_actorManager->loadActorProfile("player_explosion", "player_explosion");
}

int LevelManager::getPlayerLives() const
{
	auto healthComp = m_actorManager->getActor(m_playerId)->getComponent<Comp_Health>(ComponentType::Health);
	if (!healthComp) return 0;
	return healthComp->getHealth();
}

void LevelManager::onInvaderDefeated()
{
	m_kills++;
	--m_remainingInvaders;
}

sf::Vector2f LevelManager::getBunkerSpawn(ActorId id)
{
	if (m_bunkerSpawn.find(id) == m_bunkerSpawn.end()) return sf::Vector2f();
	return m_bunkerSpawn[id];
}

void LevelManager::newGame(sf::FloatRect viewSpace)
{
	m_viewSpace = viewSpace;
	createPlayer();
	createPlayerExplosion();
	createPlayerBullets();
	createInvaderBullets();
	createInvaders(viewSpace);
	createShockwaves();
	createBunkers(viewSpace);
}

void LevelManager::purge()
{
	m_playerLives = 0;
	m_remainingInvaders = 0;
	m_kills = 0;
	m_invaders.clear();
	m_playerBullets.clear();
	m_invaderBullets.clear();
	m_bunkers.clear();
	m_shockwaves.clear();
	m_bunkerSpawn.clear();
	m_level = 0;
}

void LevelManager::adjustColliderToSprite(std::shared_ptr<Comp_Collision> colComp, std::shared_ptr<Comp_SpriteSheet> sprite)
{
	sf::Vector2f size = sf::Vector2f(
		sprite->getSpriteSheet()->getSpriteScale().x * sprite->getSpriteSheet()->getSpriteSize().x,
		sprite->getSpriteSheet()->getSpriteScale().y * sprite->getSpriteSheet()->getSpriteSize().y
		);
	colComp->setAABB(size);
}
