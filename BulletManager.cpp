#include "BulletManager.h"
#include "Comp_BulletControl.h"
#include "Comp_Movable.h"

BulletManager::BulletManager() :
	m_currentBullet(0)
{
}

BulletManager::~BulletManager()
{
	m_bullets.clear();
}

bool BulletManager::addBullet(Actor* bullet)
{
	if (!bullet->getComponent<Comp_BulletControl>(CompType::BulletControl)) return false;
	m_bullets.push_back(bullet);
}

void BulletManager::shootBullet(sf::Vector2f direction)
{
	Actor* bullet = m_bullets[m_currentBullet++];
	Comp_Movable* movable = bullet->getComponent<Comp_Movable>(CompType::Movable);
	movable->setVelocity(direction);
	bullet->enable();

}
