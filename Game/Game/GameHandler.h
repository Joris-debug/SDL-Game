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
	std::mt19937 m_randomNumberEngine_; // Mersenne twister MT19937
	std::vector<SDL_Texture*> m_enemyTexturesWalk_; //these textures are needed for creating Enemies
	std::vector<SDL_Texture*> m_enemyTexturesIdle_;
	std::vector<SDL_Texture*> m_enemyTexturesHit_;
	std::vector<SDL_Texture*> m_hudTextures_; //This vector stores every texture needed for printing the hud
	std::vector<SDL_Texture*> m_miscTextures_; //This vector stores all leftover textures that are needed#
	int m_waveCounter_;
	int gameLoop();
public:
	GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_);
	~GameHandler();
	int initWorld();
	void checkCurrentWave(); //Checks if the current enemy-wave has been defeated and if thats the case, triggers a new one
	bool trySpawningEnemy(); //Returns true on success and false if it fails
	void renderWorldBackground();
	void renderHud(); //This function will render healthbar, attack status, etc
	void renderEverything();
};

