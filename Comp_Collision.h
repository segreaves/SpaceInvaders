#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"

class Comp_Collision : public Comp
{
public:
	Comp_Collision() :
		Comp(),
		m_collisionX(false),
		m_collisionY(false)
	{
		m_AABB = sf::FloatRect(0.f, 0.f, 50.f, 35.f);
	}

	void setCollidingX(bool colliding)
	{
		m_collisionX = colliding;
	}

	void setCollidingY(bool colliding)
	{
		m_collisionY = colliding;
	}

	bool isCollidingX() const
	{
		return m_collisionX;
	}

	bool isCollidingY() const
	{
		return m_collisionY;
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

	void resetCollision()
	{
		m_collisionX = false;
		m_collisionY = false;
	}
private:
	sf::FloatRect m_AABB;
	bool m_collisionX, m_collisionY;
};