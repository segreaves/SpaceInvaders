#pragma once
#include "Comp.h"
#include "AudioManager.h"
#include <SFML/Audio.hpp>
#include "SoundManager.h"

class Comp_SoundEmitter : public Comp
{
public:
	void create(SoundManager* soundManager)
	{
		if (!soundManager) return;
		soundManager->loadSoundProfile("assets/profiles/soundProfiles/" + m_fileName + ".sound");
	}
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_fileName;
	}
	std::string m_fileName;
};