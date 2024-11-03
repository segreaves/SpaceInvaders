#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(TextureManager* textureManager, bool sharedMemory = true) :
	m_textureManager(textureManager),
	m_spriteScale(1.0f, 1.0f),
	m_totalFrames(0),
	m_frame(0),
	m_texture(nullptr),
	m_sharedMemory(sharedMemory),
	m_origin(OriginType::Medium)
{
}

SpriteSheet::~SpriteSheet()
{
	releaseSheet();
}

void SpriteSheet::draw(sf::RenderWindow* window)
{
	window->draw(m_sprite);
}

void SpriteSheet::frameStep()
{
	m_frame = ++m_frame % m_totalFrames;
}

void SpriteSheet::resetFrame()
{
	m_frame = 0;
}

void SpriteSheet::cropSprite()
{
	m_cropRect = sf::IntRect(
		(m_spriteSize.x * m_frame) + m_sheetPadding.x + (m_spriteSpacing.x * m_frame),
		m_sheetPadding.y,
		m_spriteSize.x,
		m_spriteSize.y
	);
	m_sprite.setTextureRect(m_cropRect);
}

int SpriteSheet::getTotalFrames() const
{
	return m_totalFrames;
}

const sf::Sprite* SpriteSheet::getSprite() const
{
	return &m_sprite;
}

const sf::Texture* SpriteSheet::getTexture() const
{
	return m_texture;
}

const sf::Vector2u& SpriteSheet::getSpriteSize() const { return m_spriteSize; }

const sf::Vector2f& SpriteSheet::getSpriteScale() const
{
	return m_spriteScale;
}

const sf::Vector2f& SpriteSheet::getSpritePosition() const { return m_sprite.getPosition(); }

void SpriteSheet::setSpriteSize(const sf::Vector2u& size)
{
	m_spriteSize = size;
	switch (m_origin)
	{
		case OriginType::Medium:
			m_sprite.setOrigin(m_spriteSize.x / 2.f, m_spriteSize.y / 2.f);
			break;
		case OriginType::Bottom:
			m_sprite.setOrigin(m_spriteSize.x / 2.f, m_spriteSize.y);
			break;
		case OriginType::Top:
			m_sprite.setOrigin(m_spriteSize.x / 2.f, 0.f);
			break;
	}
}

void SpriteSheet::setSpritePosition(const sf::Vector2f& position)
{
	m_sprite.setPosition(position);
}

void SpriteSheet::setSpriteRotation(const float& rotation)
{
	m_sprite.setRotation(rotation);
}

void SpriteSheet::setSpriteScale(const sf::Vector2f& scale)
{
	m_spriteScale = scale;
	m_sprite.setScale(m_spriteScale);
}

void SpriteSheet::setSheetPadding(const sf::Vector2f& padding)
{
	m_sheetPadding = padding;
}

void SpriteSheet::setSpriteSpacing(const sf::Vector2f& spacing)
{
	m_spriteSpacing = spacing;
}

void SpriteSheet::setSpriteOrigin(OriginType origin)
{
	m_origin = origin;
}

void SpriteSheet::setSpriteColor(const sf::Color& color)
{
	m_sprite.setColor(color);
}

void SpriteSheet::setSmooth(const bool& smooth)
{
	m_texture->setSmooth(smooth);
}

const sf::Vector2f& SpriteSheet::getSheetPadding() const { return m_sheetPadding; }

const sf::Vector2f& SpriteSheet::getSpriteSpacing() const { return m_spriteSpacing; }

const sf::IntRect& SpriteSheet::getCropRect() const { return m_cropRect; }

sf::FloatRect SpriteSheet::globalRectToPixelRect(const sf::FloatRect& rect) const
{
	sf::FloatRect pixelRect;
	pixelRect.left = (rect.left - m_sprite.getGlobalBounds().left) / m_spriteScale.x;
	pixelRect.top = (rect.top - m_sprite.getGlobalBounds().top) / m_spriteScale.y;
	pixelRect.width = rect.width / m_spriteScale.x;
	pixelRect.height = rect.height / m_spriteScale.y;
	return pixelRect;
}

bool SpriteSheet::loadSheet(const std::string& filePath)
{
	std::ifstream file;
	file.open(Utils::getWorkingDirectory() + filePath);
	if (!file.is_open())
	{
		std::cerr << "SpriteSheet::loadSheet() failed to load file: " << filePath << std::endl;
		return false;
	}
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#') continue;
		std::stringstream ss(line);
		std::string type;
		ss >> type;
		if (type == "Texture")
		{
			if (m_textureId != "")
			{
				std::cerr << "Duplicate textures in: " << filePath << std::endl;
				continue;
			}
			std::string textureId;
			ss >> textureId;
			m_textureId = textureId;
			if (m_sharedMemory)
			{
				if (!m_textureManager->requireResource(textureId))
				{
					std::cerr << "SpriteSheet could not set up the texture: " << textureId << std::endl;
					continue;
				}
				m_texture = new sf::Texture();
				m_texture = m_textureManager->getResource(m_textureId);
			}
			else
			{
				// load texture directly into separate memory
				std::string texturePath = Utils::getWorkingDirectory() + m_textureManager->getPath(m_textureId);
				m_texture = new sf::Texture();
				if (!m_texture->loadFromFile(texturePath))
				{
					std::cerr << "SpriteSheet failed to load texture: " << texturePath << std::endl;
					continue;
				}
			}
			m_sprite.setTexture(*m_texture);
		}
		else if (type == "Frames")
		{
			ss >> m_totalFrames;
		}
		else if (type == "Size")
		{
			ss >> m_spriteSize.x >> m_spriteSize.y;
			setSpriteSize(m_spriteSize);
		}
		else if (type == "Scale")
		{
			ss >> m_spriteScale.x >> m_spriteScale.y;
			m_sprite.setScale(m_spriteScale);
		}
		else if (type == "Padding")
		{
			ss >> m_sheetPadding.x >> m_sheetPadding.y;
		}
		else if (type == "Spacing")
		{
			ss >> m_spriteSpacing.x >> m_spriteSpacing.y;
		}
	}
	file.close();
	return true;
}

void SpriteSheet::releaseSheet()
{
	if (m_sharedMemory)
		m_textureManager->releaseResource(m_textureId);
	else
		delete m_texture;
	m_texture = nullptr;
}
