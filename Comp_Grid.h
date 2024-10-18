#pragma once
#include "Comp.h"
#include "Grid.h"

class Comp_Grid : public Comp
{
public:
	~Comp_Grid()
	{
		delete m_grid;
		m_grid = nullptr;
	}

	void create(float width, float height, sf::Vector2f position = sf::Vector2f(0,0))
	{
		if (m_grid)
		{
			delete m_grid;
			m_grid = nullptr;
		}
		m_grid = new Grid(m_cellSize, ceil(height / m_cellSize), ceil(width / m_cellSize), position.x, position.y);
	}

	Grid* getGrid()
	{
		return m_grid;
	}

	void setPosition(const sf::Vector2f& position)
	{
		m_grid->setPosition(position);
	}
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_cellSize;
	}

	Grid* m_grid;
	float m_cellSize;
};