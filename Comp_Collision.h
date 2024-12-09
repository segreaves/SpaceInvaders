#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"
#include "OriginType.h"

/// <summary>
/// Collision component. Defines the bounding box of an actor, and its origin.
/// </summary>
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
		std::string origin;
		ss >> m_AABB.width >> m_AABB.height  >> origin;
		if (origin == "Top")
			m_origin = OriginType::Top;
		else if (origin == "Bottom")
			m_origin = OriginType::Bottom;
		else
			m_origin = OriginType::Medium;
	}

	sf::FloatRect m_AABB;
	OriginType m_origin;
};