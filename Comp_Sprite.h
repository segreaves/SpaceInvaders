#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"
#include "Utils.h"

class Comp_Sprite : public Comp
{
public:
	void load(std::stringstream& ss) override
	{
		std::string texturePath;
		ss >> texturePath >> m_scale.x >> m_scale.y;
		if (!m_texture.loadFromFile(Utils::getWorkingDirectory() + texturePath))
		{
			std::cerr << "LevelManager failed to load texture: " << texturePath << std::endl;
			return;
		}
		m_sprite.setTexture(m_texture);
		m_sprite.setOrigin(m_texture.getSize().x / 2.f, m_texture.getSize().y / 2.f);
		m_sprite.setScale(m_scale.x, m_scale.y);
	}

	void draw(sf::RenderWindow* window)
	{
		window->draw(m_sprite);
	}

	sf::Color getDefaultColor() const
	{
		return m_defaultColor;
	}

	void setColor(const sf::Color& color)
	{
		m_sprite.setColor(color);
	}

	void setPosition(const sf::Vector2f& pos)
	{
		m_sprite.setPosition(pos);
	}

	sf::Vector2f getPosition() const
	{
		return m_sprite.getPosition();
	}

	void setScale(const sf::Vector2f& scale)
	{
		m_sprite.setScale(scale);
	}

	sf::Vector2f getSize() const
	{
		return sf::Vector2f(m_sprite.getGlobalBounds().width, m_sprite.getGlobalBounds().height);
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
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	sf::Vector2f m_scale = sf::Vector2f(1, 1);
	const sf::Color m_defaultColor = sf::Color::White;
};
