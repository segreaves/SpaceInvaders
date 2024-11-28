#pragma once
#include "AudioManager.h"
#include "StateType.h"
#include <SFML/Audio/Sound.hpp>
#include <unordered_map>
#include <SFML/Audio/Music.hpp>

struct SoundInfo
{
	SoundInfo() :
		m_name(""),
		m_volume(100.f),
		m_pitch(1.f)
	{
	}
	SoundInfo(const std::string& name, float volume, float pitch) :
		m_name(name),
		m_volume(volume),
		m_pitch(pitch)
	{
	}
	std::string m_name;
	float m_volume;
	float m_pitch;
};

class SoundManager
{
public:
	SoundManager(AudioManager* audioManager);
	~SoundManager();

	bool playSound(const std::string& name);
	bool playSound(const std::string& name, const float& volume, const float& pitch);
	bool playMusic(const std::string& name);

	void setUpSound(sf::Sound* sound, const SoundInfo* props);
	void setUpMusic(sf::Music* music, const SoundInfo* props);
	void loadSoundProfile(const std::string& filePath);
	void switchState(StateType state);
	void removeState(StateType state);

	void turnOffSound();
	void setMusic(bool musicOn);

	void purge();
private:
	void pauseAll(const StateType& state);
	void unpauseAll(const StateType& state);

	AudioManager* m_audioManager;
	std::unordered_map<StateType, std::unordered_map<std::string, std::pair<SoundInfo, sf::Sound*>>> m_audio;
	std::unordered_map<StateType, std::pair<SoundInfo, sf::Music*>> m_music;
	StateType m_currentState;
};