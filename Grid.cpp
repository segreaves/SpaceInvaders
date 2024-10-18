#include "Grid.h"
#include "Direction.h"
#include <unordered_map>
#include <algorithm>

Grid::Grid(int cellSize, int rows, int cols, float posX, float posY) :
    m_cellSize(cellSize),
    m_rows(rows),
    m_cols(cols),
    m_posX(posX),
    m_posY(posY)
{
}

bool Grid::isCellValid(const int& col, const int& row) const
{
    return col >= 0 && col < m_cols && row >= 0 && row < m_rows;
}

bool Grid::contains(float x, float y) const
{
    return x >= m_posX && x < m_posX + m_cols * m_cellSize &&
        y >= m_posY && y < m_posY + m_rows * m_cellSize;
}

sf::Vector2i Grid::getCell(const sf::Vector2f& coords) const
{
    return sf::Vector2i((coords.x - m_posX) / m_cellSize, (coords.y - m_posY) / m_cellSize);
}

sf::Vector2f Grid::getCellCoords(const sf::Vector2i& cell) const
{
    return sf::Vector2f(cell.x * m_cellSize + m_posX, cell.y * m_cellSize + m_posY);
}

std::vector<sf::Vector2i> Grid::getOverlappingCells(const sf::FloatRect& rect) const
{
    std::vector<sf::Vector2i> cells;

	sf::Vector2i topLeft = getCell(sf::Vector2f(rect.left, rect.top));
	sf::Vector2i bottomRight = getCell(sf::Vector2f(rect.left + rect.width, rect.top + rect.height));

	for (int row = topLeft.y; row <= bottomRight.y; row++)
        for (int col = topLeft.x; col <= bottomRight.x; col++)
            if (isCellValid(col, row))
                cells.emplace_back(col, row);

	return cells;
}

std::vector<sf::Vector2i> Grid::getIntersect(const Ray& ray, float maxTraversal) const
{
    std::vector<sf::Vector2i> intersect;

    // starting position for the algorithm
    sf::Vector2f pos = ray.m_start;
    sf::Vector2f dir = ray.m_dir;
    sf::Vector2f destination = ray.m_start + ray.m_dir * maxTraversal;
    sf::Vector2i cell = getCell(pos);
    sf::Vector2i destCell = getCell(destination);

    // check if the ray intersects the grid
    if (!contains(ray.m_start))
    {
        // grid boundaries
        float left = getLeftEdge();
        float right = getRightEdge();
        float top = getTopEdge();
        float bottom = getBottomEdge();

        // intersection points with the grid edges
        std::unordered_map<Direction, sf::Vector2f> edgeHits;

        // left edge
        if (ray.m_dir.x != 0)
        {
            float t = (left - ray.m_start.x) / ray.m_dir.x;
            if (t >= 0 && t < maxTraversal)
            {
                sf::Vector2f point = ray.m_start + t * ray.m_dir;
                if (point.y >= top && point.y <= bottom)
                    edgeHits[Direction::Left] = point;
            }
        }

        // right edge
        if (ray.m_dir.x != 0)
        {
            float t = (right - ray.m_start.x) / ray.m_dir.x;
            if (t >= 0 && t < maxTraversal)
            {
                sf::Vector2f point = ray.m_start + t * ray.m_dir;
                if (point.y >= top && point.y <= bottom)
                    edgeHits[Direction::Right] = point;
            }
        }

        // top edge
        if (ray.m_dir.y != 0)
        {
            float t = (top - ray.m_start.y) / ray.m_dir.y;
            if (t >= 0 && t < maxTraversal)
            {
                sf::Vector2f point = ray.m_start + t * ray.m_dir;
                if (point.x >= left && point.x <= right)
                    edgeHits[Direction::Up] = point;
            }
        }

        // bottom edge
        if (ray.m_dir.y != 0)
        {
            float t = (bottom - ray.m_start.y) / ray.m_dir.y;
            if (t >= 0 && t < maxTraversal)
            {
                sf::Vector2f point = ray.m_start + t * ray.m_dir;
                if (point.x >= left && point.x <= right)
                    edgeHits[Direction::Down] = point;
            }
        }

        // if no intersection point found
        if (edgeHits.empty())
            return intersect;

        // closest intersection point
        float minDistance = std::numeric_limits<float>::infinity();
        for (const auto& point : edgeHits)
        {
            float distance = std::sqrt(std::pow(point.second.x - ray.m_start.x, 2) + std::pow(point.second.y - ray.m_start.y, 2));
            if (distance < minDistance)
            {
                minDistance = distance;
                // update the position
                pos = point.second;
                // update the cell
                switch (point.first)
                {
                case Direction::Left:
                case Direction::Up:
                    cell = getCell(pos);
                    break;
                case Direction::Right:
                    cell = sf::Vector2i((pos.x - m_posX) / m_cellSize - 1, (pos.y - m_posY) / m_cellSize);
                    break;
                case Direction::Down:
                    cell = sf::Vector2i((pos.x - m_posX) / m_cellSize, (pos.y - m_posY) / m_cellSize - 1);
                    break;
                }
            }
        }
    }

    // traverse the grid
    bool traversing = true;
    while (traversing)
    {
        intersect.emplace_back(cell);

        // check if the cell contains the destination or we exited the grid
        if (destCell == cell)
            traversing = false;

        // calculate the next cell
        float dx = dir.x > 0 ? m_cellSize : 0;
        float dy = dir.y > 0 ? m_cellSize : 0;

        sf::Vector2f cellCoords = getCellCoords(cell);

        // horizontal intersection
        float tMaxX = dir.x != 0 ? (cellCoords.x + dx - pos.x) / dir.x : std::numeric_limits<float>::infinity();
        // vertical intersection
        float tMaxY = dir.y != 0 ? (cellCoords.y + dy - pos.y) / dir.y : std::numeric_limits<float>::infinity();

        if (tMaxX < tMaxY)
        {
            pos.x += tMaxX * dir.x;
            pos.y += tMaxX * dir.y;
            cell.x += dir.x > 0 ? 1 : -1;
        }
        else
        {
            pos.x += tMaxY * dir.x;
            pos.y += tMaxY * dir.y;
            cell.y += dir.y > 0 ? 1 : -1;
        }

        // check if we exited the grid
        if (!contains(pos))
            traversing = false;
    }

    return intersect;
}