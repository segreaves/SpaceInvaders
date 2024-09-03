#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"

class Comp_Position : public Comp
{
public:
	void load(std::stringstream& ss) override
	{
	}

	sf::Vector2f getPosition()
	{
		return m_position;
	}
	
	void setPosition(const float& x, const float& y)
	{
		m_positionPrev = m_position;
		m_position = sf::Vector2f(x, y);
	}

	void setPosition(sf::Vector2f pos)
	{
		m_positionPrev = m_position;
		m_position = pos;
	}

	void move(const float& x, const float& y)
	{
		m_positionPrev = m_position;
		m_position.x += x;
		m_position.y += y;
	}

	void move(sf::Vector2f vec)
	{
		m_positionPrev = m_position;
		m_position += vec;
	}
private:
	sf::Vector2f m_position;
	sf::Vector2f m_positionPrev;
};
