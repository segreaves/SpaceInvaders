#include "Level.h"
#include "Context.h"

Level::Level(Context* context)
{
	m_context = context;
	m_context->m_level = this;
	sf::FloatRect viewSpace = m_context->m_windowManager->getViewSpace();
	// create level bounds
	float marginWidth = 10.f;
	// Up
	m_colliders[Direction::Up] = sf::FloatRect(viewSpace.left, viewSpace.top - marginWidth, viewSpace.width, marginWidth);
	// Down
	m_colliders[Direction::Down] = sf::FloatRect(viewSpace.left, viewSpace.top + viewSpace.height, viewSpace.width, marginWidth);
	// Left
	m_colliders[Direction::Left] = sf::FloatRect(viewSpace.left, viewSpace.top, marginWidth, viewSpace.height);
	// Right
	m_colliders[Direction::Right] = sf::FloatRect(viewSpace.left + viewSpace.width - marginWidth, viewSpace.top, marginWidth, viewSpace.height);
}

Level::~Level()
{
	m_colliders.clear();
	m_context = nullptr;
}

sf::FloatRect* Level::getColliders(Direction obj)
{
	auto it = m_colliders.find(obj);
	if (it == m_colliders.end()) return nullptr;
	return &it->second;
}

std::unordered_map<Direction, sf::FloatRect>& Level::getColliders()
{
	return m_colliders;
}
