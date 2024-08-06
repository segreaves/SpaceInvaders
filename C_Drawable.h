#pragma once
#include <SFML/Graphics.hpp>
#include "C_Base.h"

class C_Drawable : public C_Base
{
public:
	virtual void draw(sf::RenderWindow* window) = 0;
	virtual void setPosition(const sf::Vector2f& pos) = 0;
	virtual const sf::Vector2f& getSize() = 0;
};
