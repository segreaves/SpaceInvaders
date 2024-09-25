#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"
#include "Utils.h"
#include "SpriteSheet.h"

class Comp_SpriteSheet : public Comp
{
public:
	Comp_SpriteSheet() : Comp(),
		m_spriteSheet(nullptr),
		m_frameDuration(0.1f),
		m_frameTime(0),
		m_fps(0),
		m_sharedMemory(true),
		m_origin(OriginType::Medium)
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
		m_spriteSheet = new SpriteSheet(textureManager, m_sharedMemory);
		m_spriteSheet->setSpriteOrigin(m_origin);
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

	bool isAnimated()
	{
		return m_spriteSheet->getTotalFrames() > 1;
	}

	void frameStep()
	{
		m_spriteSheet->frameStep();
	}

	void resetFrameStep()
	{
		m_spriteSheet->resetFrame();
	
	}

	void cropSprite()
	{
		m_spriteSheet->cropSprite();
	}

	float incrementFrameTime(const float& deltaTime)
	{
		m_frameTime += deltaTime;
		return m_frameTime;
	}

	void setFrameTime(const float& frameTime)
	{
		m_frameTime = frameTime;
	}

	float getFrameTime() const
	{
		return m_frameTime;
	}

	float getFrameDuration() const
	{
		return m_frameDuration;
	}

	void setFPS(const float& fps)
	{
		m_fps = fps;
		m_frameDuration = 1.f / m_fps;
	}

	float getFPS() const
	{
		return m_fps;
	}

	float getDefaultFPS()
	{
		return m_defaultFPS;
	}
private:
	void load(std::stringstream& ss) override
	{
		std::string memoryUsage;
		unsigned int origin = 0;
		ss >> memoryUsage >> origin >> m_sheetName;
		m_origin = (OriginType)origin;
		if (memoryUsage == "single_memory")
			m_sharedMemory = false;
	}

	std::string m_sheetName;
	SpriteSheet* m_spriteSheet;
	bool m_sharedMemory;
	OriginType m_origin;
	const sf::Color m_defaultColor = sf::Color::White;
	float m_frameDuration;
	float m_frameTime;
	float m_defaultFPS = 1.f;
	float m_fps;
};