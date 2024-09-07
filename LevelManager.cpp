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

int LevelManager::loadActorProfile(const std::string actorName)
{
	std::string fullPath = Utils::getWorkingDirectory() + "assets/profiles/" + actorName + ".dat";
	//std::cout << "loadActorProfile at: " << fullPath << std::endl;
	std::fstream file;
	file.open(fullPath);
	if (!file.is_open())
	{
		std::cerr << "LevelManager failed to open file: " << fullPath << std::endl;
		return -1;
	}
	unsigned int actorId = m_actorManager->initializeActor(actorName);
	Actor* actor = m_actorManager->getActor(actorId);
	std::string line;
	while (getline(file, line))
	{
		std::stringstream ss(line);
		std::string attr;
		ss >> attr;
		if (line[0] == '#') continue;
		if (attr == "Position")
		{
			m_actorManager->addComponent(actorId, ComponentType::Position);
			Comp_Position* pos = actor->getComponent<Comp_Position>(ComponentType::Position);
			ss >> *pos;
		}
		else if (attr == "Sprite")
		{
			m_actorManager->addComponent(actorId, ComponentType::Sprite);
			Comp_Sprite* sprite = actor->getComponent<Comp_Sprite>(ComponentType::Sprite);
			ss >> *sprite;
		}
		else if (attr == "State")
		{
			//actorManager->addComponent(actorId, ComponentType::State);
			//Comp_State* state = actor->getComponent<Comp_State>(ComponentType::State);
		}
		else if (attr == "Movement")
		{
			m_actorManager->addComponent(actorId, ComponentType::Movement);
			Comp_Movement* movement = actor->getComponent<Comp_Movement>(ComponentType::Movement);
			ss >> *movement;
		}
		else if (attr == "Control")
		{
			m_actorManager->addComponent(actorId, ComponentType::Control);
			Comp_Control* control = actor->getComponent<Comp_Control>(ComponentType::Control);
			ss >> *control;
		}
		else if (attr == "Collision")
		{
			m_actorManager->addComponent(actorId, ComponentType::Collision);
			Comp_Collision* collision = actor->getComponent<Comp_Collision>(ComponentType::Collision);
			ss >> *collision;
		}
		else if (attr == "Player")
		{
			m_actorManager->addComponent(actorId, ComponentType::Player);
			Comp_Player* player = actor->getComponent<Comp_Player>(ComponentType::Player);
			ss >> *player;
		}
		else if (attr == "Invader")
		{
			m_actorManager->addComponent(actorId, ComponentType::Invader);
			Comp_Invader* invader = actor->getComponent<Comp_Invader>(ComponentType::Invader);
			ss >> *invader;
		}
		else if (attr == "Bullet")
		{
			m_actorManager->addComponent(actorId, ComponentType::Bullet);
			Comp_Bullet* bullet = actor->getComponent<Comp_Bullet>(ComponentType::Bullet);
			ss >> *bullet;
		}
		else if (attr == "Bunker")
		{
			m_actorManager->addComponent(actorId, ComponentType::Bunker);
			Comp_Bunker* bunker = actor->getComponent<Comp_Bunker>(ComponentType::Bunker);
			ss >> *bunker;
		}
		else
			std::cerr << "Unknown attribute: " << attr << std::endl;
	}
	file.close();
	return actorId;
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
	m_playerId = loadActorProfile("player");
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
			int invaderId = loadActorProfile("invader");
			m_invaders.emplace_back(invaderId);
			float spawnX = j * m_invaderSeparation.x + offset;// center in view space
			float spawnY = (i + 1) * m_invaderSeparation.y;
			m_invaderSpawn.emplace(invaderId, sf::Vector2f(spawnX, spawnY));
		}
	}
}

void LevelManager::createBullets()
{
	for (unsigned int i = 0; i < m_nBullets; i++)
		m_bullets.push_back(loadActorProfile("bullet"));
}

void LevelManager::createBunkers(sf::FloatRect viewSpace)
{
	float offset = (viewSpace.width - m_bunkerSeparation * (m_nBunkers - 1)) / 2.f;
	float spawnY = viewSpace.height - m_bunkerSpawnHeight;
	for (int i = 0; i < m_nBunkers; i++)
	{
		int bunkerId = loadActorProfile("bunker");
		m_bunkers.emplace_back(bunkerId);
		float spawnX = i * m_bunkerSeparation + offset;// center in view space
		m_bunkerSpawn.emplace(bunkerId, sf::Vector2f(spawnX, spawnY));
	}
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
