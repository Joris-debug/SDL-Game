#include "MenuManager.h"
#include "Resources.h"
#include "SDL_image.h"
MenuManager::MenuManager()
{
	m_currentMenu = Menus::none;
	m_p_gameHandler = nullptr;
	m_p_currenWorld = nullptr;

}

MenuManager& MenuManager::getInstance()
{
	static MenuManager instance;
	return instance;
}

void MenuManager::createTextures(SDL_Renderer* renderer)
{
	SDL_Surface* p_tmpSurface = IMG_Load(RSC_SHOP_MENU);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
}

void MenuManager::deleteTextures()
{
	int numberOfElements = int(m_menuTextures.size());
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_menuTextures.back());
		m_menuTextures.pop_back();
	}
}

void MenuManager::interactWithMenu(bool mouseButtonPressed, SDL_Renderer* renderer)
{

}

bool MenuManager::openShop()
{
	if (m_currentMenu == Menus::none) {
		m_currentMenu = Menus::shop;
		return true;
	}
	return false;
}
