#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"
#include <iostream>

class Comp_Sprite : public Comp
{
public:
	Comp_Sprite() :
		Comp()
	{
		m_shape.setFillColor(sf::Color::White);
		m_shape.setSize(sf::Vector2f(50.f, 35.f));
		m_shape.setOrigin(m_shape.getSize() / 2.f);
		m_center.setFillColor(sf::Color::Red);
		m_center.setSize(sf::Vector2f(5.f, 5.f));
		m_center.setOrigin(m_center.getSize() / 2.f);
	}

	~Comp_Sprite() {}

	void awake() override {}
	void onDestroy() override {}

	void draw(sf::RenderWindow* window)
	{
		window->draw(m_shape);
		window->draw(m_center);
	}

	void setPosition(const sf::Vector2f& pos)
	{
		m_shape.setPosition(pos);
		m_center.setPosition(pos);
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
	sf::RectangleShape m_center;
};