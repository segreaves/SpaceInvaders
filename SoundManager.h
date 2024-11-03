#pragma once
#include "AudioManager.h"
#include "StateType.h"
#include <SFML/Audio/Sound.hpp>
#include <unordered_map>

struct SoundInfo
{
	SoundInfo(const std::string& name, float volume, float pitch) :
		m_name(name),
		m_volume(volume),
		m_pitch(pitch),
		m_paused(false)
	{
	}
	bool m_paused;
	std::string m_name;
	float m_volume;
	float m_pitch;
};

class SoundManager
{
public:
	SoundManager(AudioManager* audioManager);
	~SoundManager();

	void play(const std::string& soundName, bool relative);
	void setUpSound(sf::Sound* sound, const SoundInfo* props, bool relative);

	void loadSoundProfile(const std::string& filePath);
	sf::Sound* createSound(const std::string& name);
private:
	AudioManager* m_audioManager;
	std::unordered_map<StateType, std::unordered_map<std::string, std::pair<SoundInfo, sf::Sound*>>> m_audio;
	StateType m_currentState;
};