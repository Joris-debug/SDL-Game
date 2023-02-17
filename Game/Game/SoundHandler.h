#include "SDL_mixer.h"
#pragma once

class SoundHandler
{
private:
	Mix_Music* m_p_gameMusic;
	Mix_Chunk* m_p_swordSound;
	Mix_Chunk* m_p_enemyHitSound;
	Mix_Chunk* m_p_playerHitSound;
	Mix_Chunk* m_p_explosionSound;
	Mix_Chunk* m_p_clickSound;
	SoundHandler();
public:
	static SoundHandler& getInstance();
	inline void playSwordSound() { Mix_PlayChannel(-1, m_p_swordSound, 0); }
	inline void playEnemyHitSound() { Mix_PlayChannel(-1, m_p_enemyHitSound, 0); }
	inline void playPlayerHitSound() { Mix_PlayChannel(-1, m_p_playerHitSound, 0); }
	inline void playExplosionSound() { Mix_PlayChannel(-1, m_p_explosionSound, 0); }
	inline void playClickSound() { Mix_PlayChannel(-1, m_p_clickSound, 0); }
	~SoundHandler();
	SoundHandler(const SoundHandler& obj) = delete;
};

