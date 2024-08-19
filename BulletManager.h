#pragma once
#include <SFML/Graphics.hpp>
#include "Actor.h"
#include <vector>

class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	bool addBullet(Actor* bullet);
	void shootBullet(sf::Vector2f direction);
private:
	unsigned int m_currentBullet;
	std::vector<Actor*> m_bullets;
};