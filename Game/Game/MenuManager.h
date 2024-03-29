#pragma once
#include "GameHandler.h"
enum class Menus { none, start, pause, shop, gameOver, optionsDefault, optionsMultiplayer };		//These are the types of menus that can be opened

class MenuManager
{
private:
	Menus m_currentMenu;
	GameHandler* m_p_gameHandler;
	World* m_p_currenWorld;	//Needed for the shop menu
	std::vector<SDL_Texture*> m_menuTextures;
	short m_menuOpacity;
	std::string m_ip4vAddress;
	std::string m_userInput;	//Currently only needed to store the inputed IP 
public:
	MenuManager(SDL_Renderer* renderer, GameHandler* m_p_gameHandler, World* m_p_currenWorld);
	~MenuManager();
	void generateUserInput(SDL_Renderer* renderer);	//This function will let the user input an IP
	inline Menus getCurrentMenu() { return m_currentMenu; }
	inline bool checkIfMenuOpen() { return (m_currentMenu == Menus::none) ? false : true; }
	bool tryClosingMenu();	//Returns true if the menu can be closed without force
	inline void closeMenu() { m_currentMenu = Menus::none; }	//This will always force the menu to close
	GameStates interactWithMenu(bool mouseButtonPressed, SDL_Renderer* renderer, double deltaTime);	//Returns true if the menu reset the game (for the game over screen)
	bool renderGameOver(SDL_Renderer* renderer, double deltaTime);
	void renderShop(bool mouseButtonPressed, SDL_Renderer* renderer);
	void renderButton(SDL_Rect buttonBounds, TTF_Font* font, std::string displayText, SDL_Color buttonColor, SDL_Color borderColor, SDL_Renderer* renderer);
	void renderButton(SDL_Rect buttonBounds, TTF_Font* font, std::string displayText, SDL_Color buttonColor, SDL_Color borderColor, SDL_Color fontColor, SDL_Renderer* renderer);	//Same as the method before, this one just takes more parameters
	void renderStartMenu(bool mouseButtonPressed, SDL_Renderer* renderer, double deltaTime);	//Returns true if the game starts
	bool renderPauseMenu(bool mouseButtonPressed, SDL_Renderer* renderer);
	void renderOptionsMenu(bool mouseButtonPressed, SDL_Renderer* renderer);
	void renderOptionsAudio(bool mouseButtonPressed, SDL_Renderer* renderer);
	void renderOptionsMultiplayer(bool mouseButtonPressed, SDL_Renderer* renderer);
	void buyHealthPotion(int* itemBoughtCounter, int price);
	void buyMoreHealth(int* itemBoughtCounter, int price);
	void buyMoreStamina(int* itemBoughtCounter, int price);
	void findOutLocalIpAddress(); //Value gets assigned to m_ip4vAddress
	bool openMenu(Menus newMenu);	//Returns true if the window can be opened

};

