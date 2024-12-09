#pragma once
#include "Comp.h"

/// <summary>
/// Records the spring properties of an actor.
/// </summary>
class Comp_Spring : public Comp
{
public:
	void setAnchor(sf::Vector2f pos) { m_anchor = pos; }
	sf::Vector2f getAnchor() const { return m_anchor; }
	void setAttach(sf::Vector2f pos) { m_attach = pos; }
	sf::Vector2f getAttach() const { return m_attach; }
	float getStrength() const { return m_strength; }
	float getLength() const { return m_length; }
	float getDampingCoeff() const { return m_dampingCoeff; }
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_strength >> m_length >> m_dampingCoeff;
	}
	sf::Vector2f m_anchor;
	sf::Vector2f m_attach;
	float m_strength;
	float m_length;
	float m_dampingCoeff;
};
