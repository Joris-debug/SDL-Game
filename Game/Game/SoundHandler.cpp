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
    Mix_PlayMusic(m_p_gameMusic, -1);

    m_p_swordSound = Mix_LoadWAV(RSC_SOUND_SWORD);
    m_p_enemyHitSound = Mix_LoadWAV(RSC_SOUND_ENEMY_HIT);
    m_p_playerHitSound = Mix_LoadWAV(RSC_SOUND_PLAYER_HIT);
    m_p_explosionSound = Mix_LoadWAV(RSC_SOUND_EXPLOSION);
    m_p_clickSound = Mix_LoadWAV(RSC_SOUND_CLICK);

    m_musicVolume = 2;
    Mix_VolumeMusic(13);

    m_audioVolume = 10;
    Mix_VolumeMusic(64);
}

SoundHandler& SoundHandler::getInstance()
{
	static SoundHandler instance;
	return instance;
}

void SoundHandler::updateMusicVolume(__int8 value)
{
    if (int(m_musicVolume + value > 20)) {
        m_musicVolume = 20;
        goto change_volume;
    }

    if (int(m_musicVolume + value < 0)) {
        m_musicVolume = 0;
        goto change_volume;
    }

    m_musicVolume += value;

    change_volume:
    Mix_VolumeMusic(round(6.4f * m_musicVolume));
}

void SoundHandler::updateAudioVolume(__int8 value)
{
    if (int(m_audioVolume + value > 20)) {
        m_audioVolume = 20;
        goto change_volume;
    }

    if (int(m_audioVolume + value < 0)) {
        m_audioVolume = 0;
        goto change_volume;
    }

    m_audioVolume += value;

    change_volume:
    Mix_Volume(-1, round(6.4f * m_audioVolume));
}

SoundHandler::~SoundHandler()
{
    Mix_FreeChunk(m_p_swordSound);
    Mix_FreeChunk(m_p_enemyHitSound);
    Mix_FreeChunk(m_p_playerHitSound);
    Mix_FreeChunk(m_p_explosionSound);
    Mix_FreeChunk(m_p_clickSound);
    Mix_FreeMusic(m_p_gameMusic);
    Mix_Quit();
}
