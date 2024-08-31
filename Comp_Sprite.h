#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"
#include "Utils.h"

class Comp_Sprite : public Comp
{
public:
	Comp_Sprite() :
		Comp()
	{
		m_texture.loadFromFile(Utils::getWorkingDirectory() + "/assets/graphics/green_square.png");
		m_sprite.setTexture(m_texture);
		m_shape.setFillColor(m_defaultColor);
		m_shape.setSize(sf::Vector2f(50.f, 50.f));
		m_shape.setOrigin(m_shape.getSize() / 2.f);
	}

	~Comp_Sprite() {}

	void awake() override {}
	void onDestroy() override {}

	void draw(sf::RenderWindow* window)
	{
		window->draw(m_shape);
	}

	sf::Color getDefaultColor() const
	{
		return m_defaultColor;
	}

	void setColor(const sf::Color& color)
	{
		m_shape.setFillColor(color);
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
		m_shape.setOrigin(m_shape.getSize() / 2.f);
	}

	sf::Vector2f getSize() const
	{
		return m_shape.getSize();
	}

	sf::Sprite* getSprite()
	{
		return &m_sprite;
	}

	sf::FloatRect getDrawableBounds()
	{
		sf::Vector2f pos = getPosition();
		sf::Vector2f size = getSize();
		return sf::FloatRect(pos.x - size.x / 2.f, pos.y - size.y / 2.f, size.x, size.y);
	}
private:
	sf::RectangleShape m_shape;
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	const sf::Color m_defaultColor = sf::Color::White;
};
