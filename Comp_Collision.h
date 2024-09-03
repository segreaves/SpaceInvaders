#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"

class Comp_Collision : public Comp
{
public:
	void load(std::stringstream& ss) override
	{
		ss >> m_AABB.width >> m_AABB.height;
	}

	void setPosition(const sf::Vector2f& pos)
	{
		m_AABB.left = pos.x - m_AABB.width / 2.f;
		m_AABB.top = pos.y - m_AABB.height / 2.f;
	}

	void setSize(const sf::Vector2f& size)
	{
		m_AABB.width = size.x;
		m_AABB.height = size.y;
	}

	void setAABB(const sf::Vector2f& size)
	{
		m_AABB.width = size.x;
		m_AABB.height = size.y;
	}

	const sf::FloatRect& getAABB() const
	{
		return m_AABB;
	}
private:
	sf::FloatRect m_AABB;
};