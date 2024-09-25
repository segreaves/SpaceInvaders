#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"
#include "OriginType.h"

class Comp_Collision : public Comp
{
public:
	Comp_Collision() :
		Comp(),
		m_origin(OriginType::Medium)
	{
	}

	void setPosition(const sf::Vector2f& pos)
	{
		switch (m_origin)
		{
		case OriginType::Medium:
			m_AABB.left = pos.x - m_AABB.width / 2.f;
			m_AABB.top = pos.y - m_AABB.height / 2.f;
			break;
		case OriginType::Bottom:
			m_AABB.left = pos.x - m_AABB.width / 2.f;
			m_AABB.top = pos.y - m_AABB.height;
			break;
		case OriginType::Top:
			m_AABB.left = pos.x - m_AABB.width / 2.f;
			m_AABB.top = pos.y;
			break;
		}
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
	void load(std::stringstream& ss) override
	{
		unsigned int origin = 0;
		ss >> m_AABB.width >> m_AABB.height  >> origin;
		m_origin = (OriginType)origin;
	}

	sf::FloatRect m_AABB;
	OriginType m_origin;
};