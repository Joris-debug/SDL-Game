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
	MenuManager();
public:
	static MenuManager& getInstance();
	void createTextures(SDL_Renderer* renderer);
	void deleteTextures();
	inline bool checkIfMenuOpen() { return (m_currentMenu == Menus::none) ? false : true; }
	inline void setGameHandler(GameHandler* m_p_gameHandler) { this->m_p_gameHandler = m_p_gameHandler; };
	inline void setCurrentWorld(World* m_p_currenWorld) { this->m_p_currenWorld = m_p_currenWorld; };
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

