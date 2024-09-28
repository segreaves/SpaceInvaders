#pragma once
#include "Comp.h"
#include <SFML/System/Vector2.hpp>

class Comp_Invader : public Comp
{
public:
	float getTimeToShoot() { return m_timeToShoot; }
	void setTimeToShoot(float timeToShoot) { m_timeToShoot = timeToShoot; }
	void decreaseTimeToShoot(float deltaTime) { m_timeToShoot -= deltaTime; }

	bool canShoot() { return m_canShoot; }
	void setCanShoot(bool canShoot) { m_canShoot = canShoot; }
private:
	void load(std::stringstream& ss) override
	{
	}
	float m_timeToShoot = 0.0f;
	bool m_canShoot = false;
};