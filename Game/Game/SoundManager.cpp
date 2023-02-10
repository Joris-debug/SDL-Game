#include "SoundManager.h"

SoundManager::SoundManager()
{
}

SoundManager& SoundManager::getInstance()
{
	static SoundManager instance;
	return instance;
}
