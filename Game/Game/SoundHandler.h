#include "SDL_mixer.h"
#pragma once

class SoundHandler
{
private:
	Mix_Music* m_p_gameMusic;
	Mix_Chunk* gScratch;
	SoundHandler();
public:
	static SoundHandler& getInstance();
	SoundHandler(const SoundHandler& obj) = delete;
};

