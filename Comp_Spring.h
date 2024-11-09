#pragma once
#include "Comp.h"

class Comp_Spring : public Comp
{
public:
	void setAnchor(sf::Vector2f pos) { m_anchor = pos; }
	sf::Vector2f getAnchor() const { return m_anchor; }
	sf::Vector2f getAttachOffset() const { return m_attachOffset; }
	float getStrength() const { return m_strength; }
	float getLength() const { return m_length; }
	float getDampingCoeff() const { return m_dampingCoeff; }
	sf::Vector2f getAttachPos(const sf::Vector2f& pos, const float& angle) const
	{
		return pos + sf::Vector2f(m_attachOffset.x * cos(angle) - m_attachOffset.y * sin(angle), m_attachOffset.x * sin(angle) + m_attachOffset.y * cos(angle));
	}
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_attachOffset.x >> m_attachOffset.y >> m_strength >> m_length >> m_dampingCoeff;
	}
	sf::Vector2f m_anchor;
	sf::Vector2f m_attachOffset;
	float m_strength;
	float m_length;
	float m_dampingCoeff;
};
