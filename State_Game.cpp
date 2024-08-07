#include "State_Game.h"
#include "StateManager.h"
#include "C_Sprite.h"

State_Game::State_Game(StateManager* stateManager) :
	State(stateManager)
{
}

void State_Game::update(const float& deltaTime)
{
	Context* context = m_stateManager->getContext();
	context->m_systemManager->update(deltaTime);
}

void State_Game::draw()
{
	Context* context = m_stateManager->getContext();
	context->m_systemManager->draw(context->m_windowManager);
}

void State_Game::onCreate()
{
	Bitmask mask((unsigned int)Component::Sprite);
	m_stateManager->getContext()->m_entityManager->addEntity(mask);
	m_stateManager->getContext()->m_entityManager->getEntity(0)->getComponent<C_Sprite>(mask)->setPosition(sf::Vector2f(100.f, 100.f));
}

void State_Game::onDestroy()
{
}

void State_Game::activate()
{
}

void State_Game::deactivate()
{
}
