#pragma once
#include <SFML/Graphics.hpp>
#include "Params.h"

class IDrawable
{
public:
	virtual void updatePosition(const sf::Vector2f& position) = 0;
	virtual void draw(sf::RenderWindow* window) = 0;
	virtual sf::FloatRect getDrawableBounds() = 0;

	sf::Color getDefaultColor() const { return m_color; }
private:
	sf::Color m_color = APP_COLOR;
};