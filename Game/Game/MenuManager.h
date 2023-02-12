#pragma once
#include "GameHandler.h"
enum class Menus { none, start, pause, shop, gameOver };		//These are the types of menus that can be opened

class MenuManager
{
private:
	Menus m_currentMenu;
	GameHandler* m_p_gameHandler;
	World* m_p_currenWorld;	//Needed for the shop menu
	std::vector<SDL_Texture*> m_menuTextures;
public:
	MenuManager(SDL_Renderer* renderer, GameHandler* m_p_gameHandler, World* m_p_currenWorld);
	~MenuManager();
	inline bool checkIfMenuOpen() { return (m_currentMenu == Menus::none) ? false : true; }
	void interactWithMenu(bool mouseButtonPressed, SDL_Renderer* renderer);
	void renderGameOver(SDL_Renderer* renderer);
	void renderShop(bool mouseButtonPressed, SDL_Renderer* renderer);
	void renderButton(SDL_Rect buttonBounds, TTF_Font* font, std::string displayText, SDL_Color buttonColor, SDL_Color borderColor, SDL_Renderer* renderer);
	void buyHealthPotion(int* itemBoughtCounter, int price);
	void buyMoreHealth(int* itemBoughtCounter, int price);
	void buyMoreStamina(int* itemBoughtCounter, int price);
	MenuManager(const MenuManager& obj)	= delete;
	inline void closeMenu() { m_currentMenu = Menus::none; }
	bool openShop();	//Returns true if the window can be opened
	inline void openGameOver() { m_currentMenu = Menus::gameOver; }	//Nothing stops the Game Over screen
};

