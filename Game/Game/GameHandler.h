#include "SDL.h"
#include <iostream>
#include <vector>
#include <random>
#pragma once
class Interface;
class World;
class Serial;
class GameHandler	
{
private:
	Serial* m_p_serial_;
	std::string m_lastDirection_;
	Interface *m_p_interface_;
	float m_deltaTime_;
	std::unique_ptr<World> m_p_currentWorld_;
	SDL_Renderer* m_p_renderer_;
	std::mt19937 m_randomNumberEngine_; // Mersenne twister MT19937
	std::vector<SDL_Texture*> m_enemyTexturesWalk_; //these textures are needed for creating Enemies
	std::vector<SDL_Texture*> m_enemyTexturesIdle_;
	std::vector<SDL_Texture*> m_enemyTexturesHit_;
	std::vector<SDL_Texture*> m_hudTextures_; //This vector stores every texture needed for printing the hud
	int gameLoop();
public:
	GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_);
	~GameHandler();
	int initLevel1();
	void renderHud(); //This function will render healthbar, attack status, etc
	void renderEverything();
};

