#pragma once
class SoundHandler
{
private:
	SoundHandler();
public:
	static SoundHandler& getInstance();
	SoundHandler(const SoundHandler& obj) = delete;
};

