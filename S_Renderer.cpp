#include "S_Renderer.h"
#include "Component.h"
#include "SystemManager.h"
#include "EntityManager.h"

S_Renderer::S_Renderer(SystemManager* systemManager)
	: S_Base(SystemType::Renderer, systemManager)
{
	Bitmask req;
	req.set((unsigned int)Component::Position, true);
	req.set((unsigned int)Component::Sprite, true);
	m_requirements.push_back(req);

	// subscribe to event channels here
}

void S_Renderer::update(const float& deltaTime)
{
	for (auto& entity : m_entities)
	{
		C_Position* positionComp = m_systemManager->getEntityManager()->getEntity(entity)->getComponent<C_Position>((unsigned int)Component::Position);
		C_Sprite* spriteComp = m_systemManager->getEntityManager()->getEntity(entity)->getComponent<C_Sprite>((unsigned int)Component::Sprite);
		if (!positionComp || !spriteComp) continue;
		spriteComp->setPosition(positionComp->getPosition());
	}
}

void S_Renderer::handleEvent(const EntityId& entityId, const EntityEvent& msg)
{
}

void S_Renderer::notify(const Message& msg)
{
}

void S_Renderer::draw(WindowManager* windowManager)
{
	for (auto& entity : m_entities)
	{
		C_Position* positionComp = m_systemManager->getEntityManager()->getEntity(entity)->getComponent<C_Position>((unsigned)Component::Position);
		C_Sprite* spriteComp = m_systemManager->getEntityManager()->getEntity(entity)->getComponent<C_Sprite>((unsigned int)Component::Sprite);
		if (!positionComp || !spriteComp) continue;
		sf::FloatRect drawableBounds;
		drawableBounds.left = positionComp->getPosition().x - spriteComp->getSize().x / 2.f;
		drawableBounds.top = positionComp->getPosition().y - spriteComp->getSize().y / 2.f;
		drawableBounds.width = spriteComp->getSize().x;
		drawableBounds.height = spriteComp->getSize().y;
		if (windowManager->getViewSpace().intersects(drawableBounds))
			spriteComp->draw(windowManager->getRenderWindow());
	}
}
