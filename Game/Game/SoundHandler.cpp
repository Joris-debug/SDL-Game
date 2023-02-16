#include "SoundHandler.h"
#include "Resources.h"
#include <iostream>

SoundHandler::SoundHandler()
{
    //Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: \n" << Mix_GetError() << std::endl;
    }

    m_p_gameMusic = Mix_LoadMUS(RSC_SOUNDTRACK);
    Mix_VolumeMusic(16);
    Mix_PlayMusic(m_p_gameMusic, -1);

    m_p_swordSound = Mix_LoadWAV(RSC_SOUND_SWORD);
    m_p_enemyHitSound = Mix_LoadWAV(RSC_SOUND_ENEMY_HIT);
    m_p_explosionSound = Mix_LoadWAV(RSC_SOUND_EXPLOSION);
}

SoundHandler& SoundHandler::getInstance()
{
	static SoundHandler instance;
	return instance;
}

SoundHandler::~SoundHandler()
{
    Mix_FreeChunk(m_p_swordSound);
    Mix_FreeChunk(m_p_enemyHitSound);
    Mix_Quit();
}
