#include "SDL.h"
#include "SDL_ttf.h"
#include <iostream>
#include <vector>
#include <random>
#pragma once

class Clock;
class Interface;
class World;
class MenuManager;
enum class gameStates {isStarting, isRunning, hasEnded};

class GameHandler	
{
private:
	MenuManager* m_p_menuManager;
	gameStates m_gameState;
	float m_deltaTime;
	int m_waveCounter;
	int m_waveTimer; //Decreasing every second the wave is active
	Clock* m_p_waveClock; //Used to detect whenever a second passes
	World* m_p_currentWorld;
	SDL_Renderer* m_p_renderer;
	std::mt19937* m_p_randomNumberEngine; // Mersenne twister MT19937
	std::vector<SDL_Texture*> m_enemyTexturesWalk; //these textures are needed for creating Enemies
	std::vector<SDL_Texture*> m_enemyTexturesIdle;
	std::vector<SDL_Texture*> m_enemyTexturesHit;
	std::vector<SDL_Texture*> m_effectTextures; //This vector stores every texture needed showing effects
	std::vector<SDL_Texture*> m_hudTextures; //This vector stores every texture needed for printing the hud
	std::vector<SDL_Texture*> m_miscTextures; //This vector stores all leftover textures that are needed
	std::vector<TTF_Font*> m_gameFonts; //This Vector stores every font the game needs
	int gameLoop();
public:
	GameHandler(SDL_Renderer* m_p_renderer_);
	~GameHandler();
	int initWorld();
	void resetWorld();
	TTF_Font* getFont(int fontSize);
	void checkCurrentWave(); //Checks if the current enemy-wave has been defeated and if thats the case, triggers a new one
	bool trySpawningEnemy(); //Returns true on success and false if it fails
	void renderWorldBackground();
	void renderHud(); //This function will render healthbar, attack status, etc
	void renderEverything(bool leftMouseButtonPressed);
	inline int getWaveCounter() { return m_waveCounter; }
};

