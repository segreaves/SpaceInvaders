#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"

/// <summary>
/// Allows an actor to be given a position in the game world.
/// </summary>
class Comp_Position : public Comp
{
public:
	sf::Vector2f getPosition() { return m_position; }

	float getAngle() { return m_angle; }
	void setAngle(const float& angle) { m_angle = angle; }

	void setPosition(sf::Vector2f pos) { m_position = pos; }
	void setPosition(const float& x, const float& y) { m_position = sf::Vector2f(x, y); }

	void move(const float& x, const float& y)
	{
		m_position.x += x;
		m_position.y += y;
	}

	void move(sf::Vector2f vec)
	{
		m_position += vec;
	}

	void rotate(float angle)
	{
		m_angle += angle;
	}
private:
	void load(std::stringstream& ss) override
	{
	}
	sf::Vector2f m_position;
	float m_angle = 0;
};
