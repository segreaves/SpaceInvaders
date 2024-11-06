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

	bool play(const std::string& name);

	void setUpSound(sf::Sound* sound, const SoundInfo* props);
	void loadSoundProfile(const std::string& filePath);
	void switchState(StateType state);
	void removeState(StateType state);

	void purge();
private:
	void pauseAll(const StateType& state);
	void unpauseAll(const StateType& state);

	AudioManager* m_audioManager;
	std::unordered_map<StateType, std::unordered_map<std::string, std::pair<SoundInfo, sf::Sound*>>> m_audio;
	StateType m_currentState;
};