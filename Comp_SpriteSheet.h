#pragma once
#include <SFML/Graphics.hpp>
#include "IDrawable.h"
#include "Utils.h"
#include "SpriteSheet.h"

class Comp_SpriteSheet : public Comp, public IDrawable
{
public:
	Comp_SpriteSheet() :
		IDrawable(),
		m_enabled(true),
		m_spriteSheet(nullptr),
		m_frameDuration(0.1f),
		m_frameTime(0),
		m_fps(0),
		m_sharedMemory(true),
		m_origin(OriginType::Medium),
		m_dmgBlinks(0)
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
		if (m_spriteSheet->loadSheet("assets/profiles/" + m_sheetName + ".sheet"))
		{
			m_spriteSheet->setSpriteColor(getDefaultColor());
			m_spriteSheet->setSmooth(false);
		}
	}

	SpriteSheet* getSpriteSheet()
	{
		return m_spriteSheet;
	}

	void updatePosition(const sf::Vector2f& position)
	{
		m_spriteSheet->setSpritePosition(position);
	}

	void draw(sf::RenderWindow* window)
	{
		if (!m_spriteSheet || !m_enabled) return;
		m_spriteSheet->draw(window);
	}

	sf::FloatRect getDrawableBounds()
	{
		return m_spriteSheet->getSprite()->getGlobalBounds();
	}

	bool isAnimated()
	{
		return m_spriteSheet->getTotalFrames() > 1;
	}

	bool isEnabled()
	{
		return m_enabled;
	}

	void setEnabled(bool enabled)
	{
		m_enabled = enabled;
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

	void startDmgBlink()
	{
		m_enabled = false;
		m_dmgBlinkTime = 0;
		m_dmgBlinks = 0;
		m_isBlinking = true;
	}

	void stopDmgBlink()
	{
		m_isBlinking = false;
	}

	float incrementBlinkTime(const float& deltaTime)
	{
		m_dmgBlinkTime += deltaTime;
		return m_dmgBlinkTime;
	}

	bool m_isBlinking;
	float m_dmgBlinkTime;
	int m_dmgBlinks;
private:
	void load(std::stringstream& ss) override
	{
		std::string memoryUsage;
		std::string origin;
		ss >> memoryUsage >> m_sheetName >> origin;
		if (memoryUsage == "single_memory")
			m_sharedMemory = false;
		if (origin == "Top")
			m_origin = OriginType::Top;
		else if (origin == "Bottom")
			m_origin = OriginType::Bottom;
		else
			m_origin = OriginType::Medium;
	}

	bool m_enabled;
	std::string m_sheetName;
	SpriteSheet* m_spriteSheet;
	bool m_sharedMemory;
	OriginType m_origin;
	float m_frameDuration;
	float m_frameTime;
	float m_defaultFPS = 1.f;
	float m_fps;
};