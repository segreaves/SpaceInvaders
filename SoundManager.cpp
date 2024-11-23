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

bool SoundManager::playSound(const std::string& name)
{
	// check audio map for sound info and sound
	const auto& stateSounds = m_audio[m_currentState];
	const auto& sound = stateSounds.find(name);
	if (sound == stateSounds.end()) return false;
	// sound exists, set up and play
	const auto& soundInfo = sound->second.first;
	const auto& snd = sound->second.second;
	setUpSound(snd, &soundInfo);
	snd->play();
	return true;
}

bool SoundManager::playMusic(const std::string& name)
{
	// check audio map for music info and music
	const auto& stateMusic = m_music[m_currentState];
	const auto& soundInfo = stateMusic.first;
	const auto& msc = stateMusic.second;
	setUpMusic(msc, &soundInfo);
	msc->play();
	return true;
}

void SoundManager::setUpSound(sf::Sound* sound, const SoundInfo* props)
{
	sound->setVolume(props->m_volume);
	sound->setPitch(props->m_pitch);
	sound->setRelativeToListener(false);
}

void SoundManager::setUpMusic(sf::Music* music, const SoundInfo* props)
{
	music->setVolume(props->m_volume);
	music->setPitch(props->m_pitch);
	music->setRelativeToListener(false);
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
		std::string type, name;
		float volume, pitch;
		ss >> type >> name >> volume >> pitch;
		SoundInfo soundInfo(name, volume, pitch);
		if (type == "sound")
		{
			if (m_audio.find(m_currentState) == m_audio.end()) return;
			auto it = m_audio[m_currentState].find(name);
			if (it == m_audio[m_currentState].end())
			{
				if (!m_audioManager->requireResource(name))
				{
					std::cerr << "SoundEngine::loadSoundProfile() failed to load sound " << name << std::endl;
					continue;
				}
				m_audio[m_currentState].emplace(name, std::make_pair(soundInfo, new sf::Sound(*m_audioManager->getResource(name))));
			}
		}
		else if (type == "music")
		{
			if (m_music.find(m_currentState) == m_music.end()) return;
			std::string musicPath = m_audioManager->getPath(name);
			if (musicPath == "") return;
			auto music = new sf::Music();
			if (!music->openFromFile(Utils::getWorkingDirectory() + musicPath))
			{
				std::cerr << "SoundManager::loadSoundProfile() failed to load music from file: " << name << std::endl;
				return;
			}
			music->setLoop(true);
			m_music[m_currentState].first = soundInfo;
			m_music[m_currentState].second = music;
		}
	}
}

void SoundManager::switchState(StateType state)
{
	pauseAll(m_currentState);
	m_currentState = state;
	unpauseAll(m_currentState);

	if (m_music.find(m_currentState) != m_music.end()) return;
	SoundInfo info = SoundInfo();
	sf::Music* music = nullptr;
	m_music.emplace(m_currentState, std::make_pair(info, music));
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

void SoundManager::turnOffSound()
{
	auto audio = m_audio.find(m_currentState);
	if (audio == m_audio.end()) return;
	if (audio->second.empty()) return;
	for (auto& sound : audio->second)
		sound.second.second->setVolume(0.f);
}

void SoundManager::setMusic(bool musicOn)
{
	auto music = m_music.find(m_currentState);
	if (music == m_music.end()) return;
	if (!music->second.second) return;
	music->second.second->setVolume(musicOn ? music->second.first.m_volume : 0.f);
}

void SoundManager::purge()
{
	// clear sounds
	for (auto& state : m_audio)
	{
		for (auto& sound : state.second)
		{
			m_audioManager->releaseResource(sound.second.first.m_name);
			delete sound.second.second;
		}
	}
	m_audio.clear();
	// clear music
	for (auto& music : m_music)
		delete music.second.second;
	m_music.clear();
}

void SoundManager::pauseAll(const StateType& state)
{
	// pause sounds
	auto& soundMap = m_audio[state];
	for (auto& [str, soundData] : soundMap)
	{
		if (soundData.second->getStatus() == sf::Sound::Playing)
			soundData.second->pause();
	}
	// pause music
	auto music = m_music.find(state);
	if (music == m_music.end()) return;
	if (!music->second.second) return;
	if (music->second.second->getStatus() == sf::Sound::Playing)
		music->second.second->pause();
}

void SoundManager::unpauseAll(const StateType& state)
{
	// unpause sounds
	auto& soundMap = m_audio[state];
	for (auto& [str, soundData] : soundMap)
	{
		if (soundData.second->getStatus() == sf::Sound::Paused)
			soundData.second->play();
	}
	// unpause music
	auto music = m_music.find(state);
	if (music == m_music.end()) return;
	if (!music->second.second) return;
	if (music->second.second->getStatus() == sf::Sound::Paused)
		music->second.second->play();
}
