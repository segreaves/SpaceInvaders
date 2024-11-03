#pragma once
#include "ResourceManager.h"
#include <SFML/Audio/SoundBuffer.hpp>

class AudioManager : public ResourceManager<AudioManager, sf::SoundBuffer>
{
public:
	AudioManager() : ResourceManager("assets/profiles/audio.dat")
	{
	}
	sf::SoundBuffer* load(const std::string& path)
	{
		sf::SoundBuffer* sound = new sf::SoundBuffer();
		if (!sound->loadFromFile(Utils::getWorkingDirectory() + path))
		{
			delete sound;
			sound = nullptr;
			std::cerr << "AudioManager() failed to load sound: " << path << std::endl;
		}
		return sound;
	}
};
