#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(TextureManager* textureManager, bool sharedMemory) :
	m_textureManager(textureManager),
	m_origin(OriginType::Medium),
	m_texture(nullptr),
	m_textureId(""),
	m_sprite(nullptr),
	m_spriteSize(1, 1),
	m_spriteScale(1.0f, 1.0f),
	m_sheetPadding(1.0f, 1.0f),
	m_spriteSpacing(1.0f, 1.0f),
	m_cropRect( {0, 0}, {1, 1 }),
	m_totalFrames(0),	
	m_frame(0), 
	m_sharedMemory(sharedMemory)
{
}

SpriteSheet::~SpriteSheet()
{
	releaseSheet();
	if (m_sprite)
	{
		delete m_sprite;
		m_sprite = nullptr;
	}
}

void SpriteSheet::draw(sf::RenderWindow* window)
{
	if (m_sprite)
	{
		window->draw(*m_sprite);
	}
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
		sf::Vector2i(
			(m_spriteSize.x * m_frame) + static_cast<int>(m_sheetPadding.x) + (static_cast<int>(m_spriteSpacing.x) * m_frame),
			static_cast<int>(m_sheetPadding.y)
		),
		sf::Vector2i(m_spriteSize.x, m_spriteSize.y)
	);
	if (m_sprite)
	{
		m_sprite->setTextureRect(m_cropRect);
	}
}

int SpriteSheet::getTotalFrames() const
{
	return m_totalFrames;
}

const sf::Sprite* SpriteSheet::getSprite() const
{
	return m_sprite;
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

sf::Vector2f SpriteSheet::getSpritePosition() const
{
	if (m_sprite)
	{
		return m_sprite->getPosition();
	}
	return sf::Vector2f();
}

void SpriteSheet::setSpriteSize(const sf::Vector2u& size)
{
	m_spriteSize = size;
	if (m_sprite)
	{
		switch (m_origin)
		{
			case OriginType::Medium:
				m_sprite->setOrigin(sf::Vector2f(m_spriteSize.x / 2.f, m_spriteSize.y / 2.f));
				break;
			case OriginType::Bottom:
				m_sprite->setOrigin(sf::Vector2f(m_spriteSize.x / 2.f, static_cast<float>(m_spriteSize.y)));
				break;
			case OriginType::Top:
				m_sprite->setOrigin(sf::Vector2f(m_spriteSize.x / 2.f, 0.f));
				break;
		}
	}
}

void SpriteSheet::setSpritePosition(const sf::Vector2f& position)
{
	if (m_sprite)
	{
		m_sprite->setPosition(position);
	}
}

void SpriteSheet::setSpriteRotation(const float& rotation)
{
	if (m_sprite)
	{
		m_sprite->setRotation(sf::degrees(rotation));
	}
}

void SpriteSheet::setSpriteScale(const sf::Vector2f& scale)
{
	m_spriteScale = scale;
	if (m_sprite)
	{
		m_sprite->setScale(m_spriteScale);
	}
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
	if (m_sprite)
	{
		m_sprite->setColor(color);
	}
}

void SpriteSheet::setSmooth(const bool& smooth)
{
	if (m_texture)
	{
		m_texture->setSmooth(smooth);
	}
}

const sf::Vector2f& SpriteSheet::getSheetPadding() const { return m_sheetPadding; }

const sf::Vector2f& SpriteSheet::getSpriteSpacing() const { return m_spriteSpacing; }

const sf::IntRect& SpriteSheet::getCropRect() const { return m_cropRect; }

sf::FloatRect SpriteSheet::globalRectToPixelRect(const sf::FloatRect& rect) const
{
	sf::FloatRect pixelRect;
	if (m_sprite)
	{
		pixelRect.position.x = (rect.position.x - m_sprite->getGlobalBounds().position.x) / m_spriteScale.x;
		pixelRect.position.y = (rect.position.y - m_sprite->getGlobalBounds().position.y) / m_spriteScale.y;
		pixelRect.size.x = rect.size.x / m_spriteScale.x;
		pixelRect.size.y = rect.size.y / m_spriteScale.y;
	}
	return pixelRect;
}

bool SpriteSheet::loadSheet(const std::string& filePath)
{
	std::ifstream file;
	file.open(Utils::getAssetsDirectory() + filePath);
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
				std::string texturePath = Utils::getAssetsDirectory() + m_textureManager->getPath(m_textureId);
				m_texture = new sf::Texture();
				if (!m_texture->loadFromFile(texturePath))
				{
					std::cerr << "SpriteSheet failed to load texture: " << texturePath << std::endl;
					continue;
				}
			}
			if (m_sprite)
			{
				delete m_sprite;
			}
			m_sprite = new sf::Sprite(*m_texture);
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
			if (m_sprite)
			{
				m_sprite->setScale(m_spriteScale);
			}
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
