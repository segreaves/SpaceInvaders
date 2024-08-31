#include "LevelManager.h"
#include "Context.h"
#include "Utils.h"
#include <fstream>
#include <sstream>

LevelManager::LevelManager() :
	m_level(0)
{
	loadProfiles("assets/game_data.dat");
}

LevelManager::~LevelManager()
{
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

void LevelManager::loadProfiles(const std::string& path)
{
	// create file stream
	std::fstream file;
	// open file
	file.open(Utils::getWorkingDirectory() + path);
	// if file is not open
	if (!file.is_open())
	{
		// print error message
		std::cerr << "Failed to open file: " << path << std::endl;
	}
	// create string line
	std::string line;
	// while getline from file stream
	while (getline(file, line))
	{
		// parse line
		std::stringstream ss(line);
		std::string type;
		ss >> type;
		if (type == "player")
			ss >> m_playerPath;
		else if (type == "invader")
			ss >> m_invaderPath;
		else if (type == "bullet")
			ss >> m_bulletPath;
		else if (type == "bunker")
			ss >> m_bunkerPath;
		else
			std::cerr << "Unknown type: " << type << std::endl;
	}
	// close file
	file.close();
	std::cout << "Player path: " << m_playerPath << std::endl;
	std::cout << "Invader path: " << m_invaderPath << std::endl;
	std::cout << "Bullet path: " << m_bulletPath << std::endl;
	std::cout << "Bunker path: " << m_bunkerPath << std::endl;
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
