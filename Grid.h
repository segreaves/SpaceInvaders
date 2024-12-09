#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

struct Ray
{
    Ray(const sf::Vector2f& start, const sf::Vector2f& dir) :
        m_start(start),
        m_dir(dir)
    {
    }

    sf::Vector2f m_start;
    sf::Vector2f m_dir;
};

/// <summary>
/// Creates a grid of cells. This class is used in the bunker system to determine
/// damage done by bullets.
/// </summary>
class Grid
{
public:
    Grid(float cellSize, int rows, int cols, float posX = 0, float posY = 0);

    float cellSize() const { return m_cellSize; }
    int rows() const { return m_rows; }
    int cols() const { return m_cols; }

    void setPosition(float x, float y) { m_posX = x; m_posY = y; }
    void setPosition(const sf::Vector2f& position) { setPosition(position.x, position.y); }
    sf::Vector2f getPosition() const { return sf::Vector2f(m_posX, m_posY); }
    sf::FloatRect getCellRect(const int& col, const int& row) const;
    float getCellSize() const { return m_cellSize; }
    float getLeftEdge() const { return m_posX; }
    float getRightEdge() const { return m_posX + m_cols * m_cellSize; }
    float getTopEdge() const { return m_posY; }
    float getBottomEdge() const { return m_posY + m_rows * m_cellSize; }
    bool isCellValid(const int& col, const int& row) const;
    bool isCellValid(const sf::Vector2i& cell) const { return isCellValid(cell.x, cell.y); }
    bool contains(float x, float y) const;
    bool contains(const sf::Vector2f& coords) const { return contains(coords.x, coords.y); }
    sf::Vector2i getCell(const sf::Vector2f& coords) const;
    sf::Vector2f getCellCoords(const sf::Vector2i& cell) const;
    std::vector<sf::Vector2i> getIntersect(const Ray& ray, float maxTraversal = std::numeric_limits<float>::infinity()) const;
private:
    float m_cellSize;
    int m_rows, m_cols;
    float m_posX, m_posY;
};
