#include "SoundManager.h"

SoundManager::SoundManager(AudioManager* audioManager)
	: m_audioManager(audioManager),
	m_currentState(StateType::Intro)
{
}

SoundManager::~SoundManager()
{
	for (auto& state : m_audio)
		for (auto& [name, snd] : state.second)
			delete snd.second;
	m_audio.clear();
}

void SoundManager::play(const std::string& soundName, bool relative)
{
	// check audio map for sound info and sound
	auto& stateSounds = m_audio[m_currentState];
	auto sound = stateSounds.find(soundName);
	if (sound == stateSounds.end()) return;
	// sound exists, set up and play
	auto soundInfo = sound->second.first;
	auto snd = sound->second.second;
	setUpSound(snd, &soundInfo, false);
	snd->play();
}

void SoundManager::setUpSound(sf::Sound* sound, const SoundInfo* props, bool relative)
{
	sound->setVolume(props->m_volume);
	sound->setPitch(props->m_pitch);
	sound->setRelativeToListener(relative);
}

void SoundManager::loadSoundProfile(const std::string& filePath)
{
	std::ifstream file;
	std::string fullPath = Utils::getWorkingDirectory() + filePath;
	file.open(fullPath);
	if (!file.is_open())
	{
		std::cerr << "SoundEngine::loadSoundProfile() failed to open filePath " << fullPath << std::endl;
		return;
	}
	std::string line; 
	while (std::getline(file, line))
	{
		if (line[0] == '#') continue;
		std::stringstream ss(line);
		std::string name;
		float volume, pitch;
		ss >> name >> volume >> pitch;
		SoundInfo soundInfo(name, volume, pitch);
		if (!m_audioManager->requireResource(name))
		{
			std::cerr << "SoundEngine::loadSoundProfile() failed to load resource " << name << std::endl;
			return;
		}
		m_audio[m_currentState].emplace(name, std::make_pair(soundInfo, new sf::Sound(*m_audioManager->getResource(name))));
	}
}

sf::Sound* SoundManager::createSound(const std::string& name)
{
	return m_audioManager->requireResource(name) ? new sf::Sound(*m_audioManager->getResource(name)) : nullptr;
}
