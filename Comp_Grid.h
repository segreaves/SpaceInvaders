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
		m_grid = new Grid(m_cellSize, height / m_cellSize, width / m_cellSize, position.x, position.y);
		m_enabled.resize(width * height, true);
	}

	Grid* getGrid()
	{
		return m_grid;
	}

	bool isEnabled(int col, int row) const
	{
		if (!m_grid->isCellValid(col, row)) return false;
		return m_enabled[row * m_grid->cols() + col];
	}

	bool isEnabled(const sf::Vector2i& cell) const
	{
		return isEnabled(cell.x, cell.y);
	}

	void disableCell(const sf::Vector2i& cell)
	{
		m_enabled[cell.y * m_grid->cols() + cell.x] = false;
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
	int m_cellSize;
	std::vector<bool> m_enabled;
};