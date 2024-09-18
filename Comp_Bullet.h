#pragma once
#include "Comp.h"

class Comp_Bullet : public Comp
{
public:
	float getbulletSpeed() const { return m_bulletSpeed; }
	std::string getBulletTag() const { return m_bulletTag; }
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_bulletTag >> m_bulletSpeed;
	}

	float m_bulletSpeed;
	std::string m_bulletTag;
};