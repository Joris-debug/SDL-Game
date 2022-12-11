#include "SDL.h"
#include <iostream>
#include <vector>
#include <random>
#pragma once
class Interface;
class World;
class GameHandler	
{
private:
	Interface *m_p_interface_;
	float m_deltaTime_;
	std::unique_ptr<World> m_p_currentWorld_;
	SDL_Renderer* m_p_renderer_;
	double *m_p_pixel_per_pixel_;
	std::mt19937 m_randomNumberEngine_; // Mersenne twister MT19937
	std::vector<SDL_Texture*> m_enemyTexturesWalk_; //these textures are needed for creating Enemies
	std::vector<SDL_Texture*> m_enemyTexturesIdle_;
	std::vector<SDL_Texture*> m_enemyTexturesHit_;
	int gameLoop();
public:
	GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_, double *m_p_pixel_per_pixel_);
	~GameHandler();
	int initLevel1();
	void renderEverything();
};

