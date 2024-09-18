#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"
#include "Utils.h"
#include "SpriteSheet.h"

class Comp_SpriteSheet : public Comp
{
public:
	Comp_SpriteSheet() : Comp(),
		m_spriteSheet(nullptr)
	{
	}

	~Comp_SpriteSheet()
	{
		if (m_spriteSheet)
		{
			delete m_spriteSheet;
			m_spriteSheet = nullptr;
		}
	}

	void create(TextureManager* textureManager)
	{
		if (m_spriteSheet) return;
		m_spriteSheet = new SpriteSheet(textureManager);
		m_spriteSheet->loadSheet("assets/profiles/" + m_sheetName + ".sheet");
	}

	SpriteSheet* getSpriteSheet()
	{
		return m_spriteSheet;
	}

	void updatePosition(const sf::Vector2f& vec)
	{
		m_spriteSheet->setSpritePosition(vec);
	}

	const sf::Vector2u& getSize()
	{
		return m_spriteSheet->getSpriteSize();
	}

	void draw(sf::RenderWindow* window)
	{
		if (!m_spriteSheet) return;
		m_spriteSheet->draw(window);
	}

	sf::Color getDefaultColor() const
	{
		return m_defaultColor;
	}

	sf::FloatRect getDrawableBounds()
	{
		sf::Vector2f pos = m_spriteSheet->getSpritePosition();
		sf::Vector2u size = m_spriteSheet->getSpriteSize();
		return sf::FloatRect(pos.x - size.x / 2.f, pos.y - size.y / 2.f, size.x, size.y);
	}
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_sheetName;
	}

	std::string m_sheetName;
	SpriteSheet* m_spriteSheet;
	sf::Vector2f m_scale = sf::Vector2f(1, 1);
	const sf::Color m_defaultColor = sf::Color::White;
};