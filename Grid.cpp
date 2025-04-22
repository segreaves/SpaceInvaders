#include "Grid.h"
#include "Direction.h"
#include <unordered_map>
#include <algorithm>

Grid::Grid(float cellSize, int rows, int cols, float posX, float posY) :
    m_cellSize(cellSize),
    m_rows(rows),
    m_cols(cols),
    m_posX(posX),
    m_posY(posY)
{
}

sf::FloatRect Grid::getCellRect(const int& col, const int& row) const
{
    return sf::FloatRect({m_posX + col * m_cellSize, m_posY + row * m_cellSize}, {m_cellSize, m_cellSize});
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
    return sf::Vector2i(static_cast<int>((coords.x - m_posX) / m_cellSize), static_cast<int>((coords.y - m_posY) / m_cellSize));
}

sf::Vector2f Grid::getCellCoords(const sf::Vector2i& cell) const
{
    return sf::Vector2f(cell.x * m_cellSize + m_posX, cell.y * m_cellSize + m_posY);
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
        const auto left = getLeftEdge();
        const auto right = getRightEdge();
        const auto top = getTopEdge();
        const auto bottom = getBottomEdge();

        // intersection points with the grid edges
        std::unordered_map<Direction, sf::Vector2f> edgeHits;

        // left edge
        if (ray.m_dir.x != 0)
        {
            const auto t = (left - ray.m_start.x) / ray.m_dir.x;
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
            const auto t = (right - ray.m_start.x) / ray.m_dir.x;
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
            const auto t = (top - ray.m_start.y) / ray.m_dir.y;
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
            const auto t = (bottom - ray.m_start.y) / ray.m_dir.y;
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
        auto minDistance = std::numeric_limits<float>::infinity();
        for (const auto& point : edgeHits)
        {
            const auto pointX = point.second.x - ray.m_start.x;
            const auto pointY = point.second.y - ray.m_start.y;
            const auto distance = std::sqrt(pointX * pointX + pointY * pointY);
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
                    cell = sf::Vector2i(static_cast<int>((pos.x - m_posX) / m_cellSize) - 1, static_cast<int>((pos.y - m_posY) / m_cellSize));
                    break;
                case Direction::Down:
                    cell = sf::Vector2i(static_cast<int>((pos.x - m_posX) / m_cellSize), static_cast<int>((pos.y - m_posY) / m_cellSize - 1));
                    break;
                }
            }
        }
    }

    // traverse the grid
    bool traversing = true;
    float traversed = 0;
    while (traversing)
    {
        intersect.emplace_back(cell);

        // check if the cell contains the destination or we exited the grid
        if (destCell == cell)
            traversing = false;

        // calculate the next cell
        const auto dx = dir.x > 0 ? m_cellSize : 0;
        const auto dy = dir.y > 0 ? m_cellSize : 0;

        sf::Vector2f cellCoords = getCellCoords(cell);

        // horizontal intersection
        const auto tMaxX = dir.x != 0 ? (cellCoords.x + dx - pos.x) / dir.x : std::numeric_limits<float>::infinity();
        // vertical intersection
        const auto tMaxY = dir.y != 0 ? (cellCoords.y + dy - pos.y) / dir.y : std::numeric_limits<float>::infinity();

        if (tMaxX < tMaxY)
        {
            pos.x += tMaxX * dir.x;
            pos.y += tMaxX * dir.y;
            cell.x += dir.x > 0 ? 1 : -1;
            traversed += tMaxX;
        }
        else
        {
            pos.x += tMaxY * dir.x;
            pos.y += tMaxY * dir.y;
            cell.y += dir.y > 0 ? 1 : -1;
            traversed += tMaxY;
        }

        // check if we exited the grid
        if (!contains(pos) || traversed >= maxTraversal)
            traversing = false;
    }

    return intersect;
}