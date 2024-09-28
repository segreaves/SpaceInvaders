#pragma once
#include "Comp.h"

class Comp_Shockwave : public Comp
{
public:
	Comp_Shockwave() :
		m_radius(0),
		m_force(0),
		m_time(0),
		m_lifeTime(0)
	{
	}

	float getForce() const { return m_force; }

	void setRadius(const float& radius)
	{
		m_radius = radius;
	}
	float getRadius() const { return m_radius; }
	float getMaxRadius() const { return m_maxRadius; }

	float getLifeTime() const { return m_lifeTime; }
	float getTime() const { return m_time; }
	void resetTime() { m_time = 0; }
	void incrementTime(const float& deltaTime) { m_time += deltaTime; }
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_force >> m_lifeTime;
	}

	const float m_maxRadius = 200;
	float m_radius;
	float m_force;
	float m_lifeTime;
	float m_time;
};