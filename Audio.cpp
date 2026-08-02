#include "../h/Audio.h"

SoundManager* Audio::s_Instance = nullptr;

void Audio::init() {
	static SoundManager manager;
	s_Instance = &manager;
}

SoundManager& Audio::sound() {
	return *s_Instance;
}