#include "LevelManager.h"
#include "Context.h"
#include "Utils.h"
#include <fstream>
#include <sstream>

LevelManager::LevelManager() :
	m_level(0),
	m_playerId(-1)
{
}

LevelManager::~LevelManager()
{
}

void LevelManager::loadActorProfile(const std::string actorName)
{
	std::string fullPath = Utils::getWorkingDirectory() + "assets/profiles/" + actorName + ".dat";
	//std::cout << "loadActorProfile at: " << fullPath << std::endl;
	std::fstream file;
	file.open(fullPath);
	if (!file.is_open())
	{
		std::cerr << "LevelManager failed to open file: " << fullPath << std::endl;
		return;
	}
	ActorManager* actorManager = m_context->m_actorManager;
	unsigned int actorId = actorManager->initializeActor(actorName);
	Actor* actor = actorManager->getActor(actorId);
	std::string line;
	while (getline(file, line))
	{
		std::stringstream ss(line);
		std::string attr;
		ss >> attr;
		if (line[0] == '#') continue;
		if (attr == "Position")
		{
			actorManager->addComponent(actorId, ComponentType::Position);
			Comp_Position* pos = actor->getComponent<Comp_Position>(ComponentType::Position);
			ss >> *pos;
		}
		else if (attr == "Sprite")
		{
			actorManager->addComponent(actorId, ComponentType::Sprite);
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
			actorManager->addComponent(actorId, ComponentType::Movement);
			Comp_Movement* movement = actor->getComponent<Comp_Movement>(ComponentType::Movement);
			ss >> *movement;
		}
		else if (attr == "Control")
		{
			actorManager->addComponent(actorId, ComponentType::Control);
			Comp_Control* control = actor->getComponent<Comp_Control>(ComponentType::Control);
			ss >> *control;
		}
		else if (attr == "Collision")
		{
			actorManager->addComponent(actorId, ComponentType::Collision);
			Comp_Collision* collision = actor->getComponent<Comp_Collision>(ComponentType::Collision);
			ss >> *collision;
		}
		else if (attr == "Player")
		{
			actorManager->addComponent(actorId, ComponentType::Player);
			Comp_Player* player = actor->getComponent<Comp_Player>(ComponentType::Player);
			ss >> *player;
			m_playerId = actorId;
		}
		else if (attr == "Invader")
		{
			actorManager->addComponent(actorId, ComponentType::Invader);
			Comp_Invader* invader = actor->getComponent<Comp_Invader>(ComponentType::Invader);
			m_invaders.push_back(actorId);
			ss >> *invader;
		}
		else if (attr == "Bullet")
		{
			actorManager->addComponent(actorId, ComponentType::Bullet);
			Comp_Bullet* bullet = actor->getComponent<Comp_Bullet>(ComponentType::Bullet);
			ss >> *bullet;
		}
		else if (attr == "Bunker")
		{
			actorManager->addComponent(actorId, ComponentType::Bunker);
			Comp_Bunker* bunker = actor->getComponent<Comp_Bunker>(ComponentType::Bunker);
			ss >> *bunker;
		}
		else
			std::cerr << "Unknown attribute: " << attr << std::endl;
	}
	file.close();
}

std::vector<sf::Vector2f> LevelManager::getInvaderSpawnPoints()
{
	return m_invaderSpawnPoints;
}

void LevelManager::setInvaderSpawnPoints(const sf::Vector2f& invaderSize)
{
	m_invaderSpawnPoints.clear();
	m_invaderSpawnPoints = getGridFormation(m_invaderRows, m_invaderCols, invaderSize.x, invaderSize.y, m_invaderPadding);
	float gridWidth = m_invaderCols * (invaderSize.x + m_invaderPadding) - m_invaderPadding;
	float gridHeight = m_invaderRows * (invaderSize.y + m_invaderPadding) - m_invaderPadding;
	float topBuffer = 10.f;
	for (int i = 0; i < m_invaderSpawnPoints.size(); i++)
	{
		// center the formation in the view space
		m_invaderSpawnPoints[i].x += m_viewSpace.width / 2.f - gridWidth / 2.f + invaderSize.x / 2.f;
		// move the formation down on the view space
		m_invaderSpawnPoints[i].y += topBuffer + invaderSize.y / 2.f;
	}
}

std::vector<sf::Vector2f> LevelManager::getBunkerSpawnPoints()
{
	return m_bunkerSpawnPoints;
}

void LevelManager::setBunkerSpawnPoints()
{
	m_bunkerSpawnPoints.clear();
	float bunkerSpacing = m_viewSpace.width / (m_nBunkers + 1);
	float gridWidth = m_nBunkers * bunkerSpacing;
	m_bunkerSpawnPoints = getGridFormation(1, m_nBunkers, bunkerSpacing, 0, 0);
	float topBuffer = m_viewSpace.height * 0.8;
	for (int i = 0; i < m_bunkerSpawnPoints.size(); i++)
	{
		// center the formation in the view space
		m_bunkerSpawnPoints[i].x += m_viewSpace.width / 2.f - gridWidth / 2.f + bunkerSpacing / 2.f;
		// move the formation down on the view space
		m_bunkerSpawnPoints[i].y += topBuffer;
	}
}

sf::Vector2f LevelManager::getPlayerSpawnPoint() const
{
	return sf::Vector2f(m_viewSpace.left + m_viewSpace.width / 2.f, m_viewSpace.top + m_viewSpace.height * 0.9f);
}

void LevelManager::setViewSpace(sf::FloatRect viewSpace)
{
	m_viewSpace = viewSpace;
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
