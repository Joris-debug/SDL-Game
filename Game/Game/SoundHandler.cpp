#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
}

SoundHandler& SoundHandler::getInstance()
{
	static SoundHandler instance;
	return instance;
}
