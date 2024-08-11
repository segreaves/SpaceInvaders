#include "State_Game.h"
#include "StateManager.h"
#include "Direction.h"
#include "Comp_Position.h"
#include "Comp_Sprite.h"

State_Game::State_Game(StateManager* stateManager) :
	State(stateManager),
	m_playerId(0)
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
	// create player entity
	Bitmask mask;
	//mask.set((unsigned int)Component::Position);
	//mask.set((unsigned int)Component::Sprite);
	mask.set((unsigned int)CompType::Position);
	mask.set((unsigned int)CompType::Sprite);
	//m_playerId = m_stateManager->getContext()->m_entityManager->addEntity(mask);
	m_playerId = m_stateManager->getContext()->m_entityManager->createActor(mask);
	// set player in center-bottom of the view space
	sf::FloatRect viewSpace = m_stateManager->getContext()->m_windowManager->getViewSpace();
	//C_Sprite* playerSprite = m_stateManager->getContext()->m_entityManager->getEntity(m_playerId)->getComponent<C_Sprite>((unsigned)Component::Sprite);
	//m_stateManager->getContext()->m_entityManager->getEntity(0)->getComponent<C_Position>((unsigned int)Component::Position)->setPosition(viewSpace.left + viewSpace.width / 2.f - playerSprite->getSize().x / 2.f, viewSpace.top + viewSpace.height * 0.9f - playerSprite->getSize().y / 2.f);
	Comp_Sprite* playerSprite = m_stateManager->getContext()->m_entityManager->getActor(0)->getComponent<Comp_Sprite>(CompType::Sprite);
	Comp_Position* playerPos = m_stateManager->getContext()->m_entityManager->getActor(0)->getComponent<Comp_Position>(CompType::Position);
	playerPos->setPosition(viewSpace.left + viewSpace.width / 2.f - playerSprite->getSize().x / 2.f, viewSpace.top + viewSpace.height * 0.9f - playerSprite->getSize().y / 2.f);
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

/*void State_Game::playerMove(EventDetails* details)
{
	Message msg((MessageType)EntityMessage::Move);
	if (details->m_name == "Player_MoveLeft") msg.m_int = (int)Direction::Left;
	else if (details->m_name == "Player_MoveRight") msg.m_int = (int)Direction::Right;
	else if (details->m_name == "Player_MoveUp") msg.m_int = (int)Direction::Up;
	else if (details->m_name == "Player_MoveDown") msg.m_int = (int)Direction::Down;
	msg.m_receiver = m_playerId;
	m_stateManager->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
}*/
