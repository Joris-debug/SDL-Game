#include "SDL.h"
#include <iostream>
#include <vector>
#pragma once
class Interface;
class World;
class GameHandler	
{
private:
	Interface *m_p_interface_;
	double m_deltaTime_;
	std::unique_ptr<World> m_p_currentWorld_;
	SDL_Renderer* m_p_renderer_;
	double *m_p_pixel_per_pixel_;
	std::vector<SDL_Texture*> m_enemyTexturesWalk_; //these textures are needed for creating Enemies
	std::vector<SDL_Texture*> m_enemyTexturesIdle_;
	int gameLoop();
public:
	GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_, double *m_p_pixel_per_pixel_);
	~GameHandler();
	int initLevel1();
	void renderEverything();
};

