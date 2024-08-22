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

std::vector<sf::Vector2f> Level::getInvaderSpawnPoints(sf::FloatRect viewSpace)
{
	// create a grid of invaders side-by-side
	unsigned int rows = 5;
	unsigned int columns = m_numInvaders / rows;
	float gridWidth = viewSpace.width * 0.3f;
	float gridHeight = viewSpace.height * 0.2;
	std::vector<sf::Vector2f> formationPoints = getGridFormation(rows, columns, gridWidth, gridHeight);
	std::vector<sf::Vector2f> secondGrid(formationPoints);
	float offset = 0.6f;
	for (int i = 0; i < formationPoints.size(); i++)
	{
		// center the formation in the view space
		formationPoints[i].x += viewSpace.width / 2.f;
		secondGrid[i].x += viewSpace.width / 2.f;
		// move the formation to the top of the view space
		formationPoints[i].y += viewSpace.height * 0.25f;
		secondGrid[i].y += viewSpace.height * 0.25f;
		// move one grid to the left and the other to the right
		formationPoints[i].x -= offset * gridWidth;
		secondGrid[i].x += offset * gridWidth;
	}
	formationPoints.insert(formationPoints.end(), secondGrid.begin(), secondGrid.end());
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
	}
	if (columns == 1)
	{
		float rowDelta = lengthY / rows > 1 ? (rows - 1) : 1;
		for (int row = 0; row < rows; row++)
		{
			float y = -0.5f * lengthY + rowDelta * row;
			formationPoints.emplace_back(sf::Vector2f(0, y));
		}
	}
	else
	{
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
	}
	return formationPoints;
}
