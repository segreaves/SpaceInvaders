#include "Level.h"
#include "Context.h"

Level::Level(Context* context)
{
	m_context = context;
	m_context->m_level = this;
}

Level::~Level()
{
	m_context = nullptr;
}

std::vector<sf::Vector2f> Level::getInvaderSpawnPoints(sf::FloatRect viewSpace, const sf::Vector2f& invaderSize)
{
	// create a grid of invaders side-by-side
	float gridWidth = viewSpace.width * 0.3f;
	float gridHeight = viewSpace.height * 0.2;
	std::vector<sf::Vector2f> formationPoints = getGridFormation(m_invaderRows, m_invaderCols, m_invaderCols * invaderSize.x, m_invaderRows * invaderSize.y);
	float offset = 0.6f;
	for (int i = 0; i < formationPoints.size(); i++)
	{
		// center the formation in the view space
		formationPoints[i].x += viewSpace.width / 2.f;
		// move the formation to the top of the view space
		formationPoints[i].y += viewSpace.height * 0.25f;
	}
	return formationPoints;
}

sf::Vector2f Level::getPlayerSpawnPoint(sf::FloatRect viewSpace) const
{
	return sf::Vector2f(viewSpace.left + viewSpace.width / 2.f, viewSpace.top + viewSpace.height * 0.9f);
}

std::vector<sf::Vector2f> Level::getGridFormation(unsigned int rows, unsigned int columns, float lengthX, float lengthY)
{
	std::vector<sf::Vector2f> formationPoints;
	formationPoints.reserve(rows * columns);
	if (rows == 1)
	{
		float colDelta = lengthX / columns > 1 ? (columns - 1) : 1;
		for (int col = 0; col < columns; col++)
		{
			float x = -0.5f * lengthX + colDelta * col;
			formationPoints.emplace_back(sf::Vector2f(-0.5f * lengthX + colDelta * col, 0));
		}
		return formationPoints;
	}
	if (columns == 1)
	{
		float rowDelta = lengthY / rows > 1 ? (rows - 1) : 1;
		for (int row = 0; row < rows; row++)
		{
			float y = -0.5f * lengthY + rowDelta * row;
			formationPoints.emplace_back(sf::Vector2f(0, y));
		}
		return formationPoints;
	}
	float rowDelta = lengthY / (rows - 1);
	float colDelta = lengthX / (columns - 1);
	for (int row = 0; row < rows; row++)
	{
		for (int col = 0; col < columns; col++)
		{
			float x = -0.5f * lengthX + colDelta * col;
			float y = -0.5f * lengthY + rowDelta * row;
			formationPoints.emplace_back(sf::Vector2f(x, y));
		}
	}
	return formationPoints;
}
