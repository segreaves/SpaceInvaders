#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"

class Comp_Sprite : public Comp
{
public:
	Comp_Sprite()
	{
		m_shape.setFillColor(sf::Color::White);
		m_shape.setSize(sf::Vector2f(50.f, 35.f));
	}

	~Comp_Sprite() {}

	void awake() override {}
	void onDestroy() override {}

	void draw(sf::RenderWindow* window)
	{
		window->draw(m_shape);
	}

	void setPosition(const sf::Vector2f& pos)
	{
		m_shape.setPosition(pos);
	}

	sf::Vector2f getPosition() const
	{
		return m_shape.getPosition();
	}

	void setSize(const sf::Vector2f& size)
	{
		m_shape.setSize(size);
	}

	sf::Vector2f getSize() const
	{
		return m_shape.getSize();
	}

	sf::FloatRect getDrawableBounds()
	{
		sf::Vector2f pos = getPosition();
		sf::Vector2f size = getSize();
		return sf::FloatRect(pos.x - size.x / 2.f, pos.y - size.y / 2.f, size.x, size.y);
	}
private:
	sf::RectangleShape m_shape;
};
