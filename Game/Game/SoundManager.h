#pragma once
class SoundManager
{
private:
	SoundManager();
public:
	static SoundManager& getInstance();
	SoundManager(const SoundManager& obj) = delete;
};

