#pragma once
#include <SFML/Graphics.hpp>
#include "C_Base.h"

class C_Position : public C_Base
{
public:
	void ping() override
	{
		std::cout << "Position ping" << std::endl;
	}

	void move(float x, float y)
	{
		m_positionPrev = m_position;
		m_position += sf::Vector2f(x, y);
	}

	void move(const sf::Vector2f& pos)
	{
		m_positionPrev = m_position;
		m_position += pos;
	}

	const sf::Vector2f& getPosition()
	{
		return m_position;
	}

	const sf::Vector2f& getPrevPosition()
	{
		return m_positionPrev;
	}

	void setPosition(const sf::Vector2f& pos)
	{
		m_positionPrev = m_position;
		m_position = pos;
	}

	void setPosition(float x, float y)
	{
		m_positionPrev = m_position;
		m_position = sf::Vector2f(x, y);
	}
private:
	sf::Vector2f m_position;
	sf::Vector2f m_positionPrev;
};