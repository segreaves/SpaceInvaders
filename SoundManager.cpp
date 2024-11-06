#include "SoundManager.h"
#include <SFML/Audio/Listener.hpp>

SoundManager::SoundManager(AudioManager* audioManager)
	: m_audioManager(audioManager),
	m_currentState(StateType::Intro)
{
}

SoundManager::~SoundManager()
{
	purge();
}

bool SoundManager::play(const std::string& name)
{
	// check audio map for sound info and sound
	auto& stateSounds = m_audio[m_currentState];
	auto sound = stateSounds.find(name);
	if (sound == stateSounds.end()) return false;
	// sound exists, set up and play
	auto soundInfo = sound->second.first;
	auto snd = sound->second.second;
	setUpSound(snd, &soundInfo);
	snd->play();
	return true;
}

void SoundManager::setUpSound(sf::Sound* sound, const SoundInfo* props)
{
	sound->setVolume(props->m_volume);
	sound->setPitch(props->m_pitch);
	sound->setRelativeToListener(false);
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
		float volume, pitch, minDist, attenuation;
		ss >> name >> volume >> pitch;
		SoundInfo soundInfo(name, volume, pitch);
		auto it = m_audio[m_currentState].find(name);
		if (it == m_audio[m_currentState].end())
		{
			if (!m_audioManager->requireResource(name))
			{
				std::cerr << "SoundEngine::loadSoundProfile() failed to load resource " << name << std::endl;
				continue;
			}
			m_audio[m_currentState].emplace(name, std::make_pair(soundInfo, new sf::Sound(*m_audioManager->getResource(name))));
		}
	}
}

void SoundManager::switchState(StateType state)
{
	pauseAll(m_currentState);
	m_currentState = state;
	unpauseAll(m_currentState);
}

void SoundManager::removeState(StateType state)
{
	auto it = m_audio.find(state);
	if (it == m_audio.end()) return;
	for (auto& [name, soundDat] : it->second)
	{
		m_audioManager->releaseResource(name);
		soundDat.second = nullptr;
	}
	m_audio.erase(state);
}

void SoundManager::purge()
{
	for (auto& state : m_audio)
	{
		for (auto& sound : state.second)
		{
			m_audioManager->releaseResource(sound.second.first.m_name);
			delete sound.second.second;
		}
	}
	m_audio.clear();
}

void SoundManager::pauseAll(const StateType& state)
{
	auto& soundMap = m_audio[state];
	for (auto& [str, soundData] : soundMap)
	{
		if (soundData.second->getStatus() == sf::Sound::Playing)
			soundData.second->pause();
	}
}

void SoundManager::unpauseAll(const StateType& state)
{
	auto& soundMap = m_audio[state];
	for (auto& [str, soundData] : soundMap)
	{
		if (soundData.second->getStatus() == sf::Sound::Paused)
			soundData.second->play();
	}
}
