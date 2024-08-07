#pragma once
#include <SFML/Graphics.hpp>
#include "C_Drawable.h"

class C_Sprite : public C_Drawable
{
public:
	C_Sprite() : C_Drawable()
	{
		m_shape.setFillColor(sf::Color::White);
		m_shape.setSize(sf::Vector2f(75.f, 100.f));
	}

	C_Sprite(sf::RectangleShape shape) : C_Drawable(),
		m_shape(shape)
	{
	}

	void ping() override
	{
		std::cout << "Sprite ping" << std::endl;
	}

	void draw(sf::RenderWindow* window) override
	{
		window->draw(m_shape);
	}

	void setPosition(const sf::Vector2f& pos) override
	{
		m_shape.setPosition(pos);
	}

	const sf::Vector2f& getSize() override
	{
		return m_shape.getSize();
	}
private:
	sf::RectangleShape m_shape;
};
