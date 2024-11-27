#pragma once
#include "Comp.h"

class Comp_Bullet : public Comp
{
public:
	float getBulletSpeed() const { return m_bulletSpeed; }
	float getBulletFrameDuration() const { return m_bulletFrameDuration; }
	float getBulletFrameTime() const { return m_bulletFrameTime; }
	void resetBulletFrameTime() { m_bulletFrameTime = 0; }
	float getDirection() const { return m_direction; }
	float incrementBulletFrameTime(const float& deltaTime) { return m_bulletFrameTime += deltaTime; }
	std::string getBulletTag() const { return m_bulletTag; }
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_bulletTag >> m_bulletSpeed >> m_bulletFrameDuration >> m_direction;
	}

	float m_bulletSpeed = 0;
	float m_bulletFrameDuration = 0;
	float m_bulletFrameTime = 0;
	float m_direction = 0;
	std::string m_bulletTag;
};