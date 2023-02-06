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
	switch (m_currentMenu) {
		case Menus::none:
			return;
			break;
		case Menus::shop:
			renderShop(mouseButtonPressed, renderer);
			break;
	}
}

void MenuManager::renderShop(bool mouseButtonPressed, SDL_Renderer* renderer)
{

	int mousePosX, mousePosY;
	SDL_GetMouseState(&mousePosX, &mousePosY);
	SDL_Rect mouseBounds = { mousePosX, mousePosY, 10, 10 };
	SDL_RenderCopy(renderer, m_menuTextures[0], NULL, NULL);



	SDL_Color buttonBorder = { 229, 229, 203 };
	SDL_Color buttonInside = { 27, 18, 15 };

	SDL_Rect closeRectBorder = { 105, 128, 130, 40 };
	SDL_Rect closeRect = { 110, 133, 120, 29 };
	if (SDL_HasIntersection(&mouseBounds, &closeRectBorder)) {
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {
			m_currentMenu = Menus::none;
		}
	}



	SDL_SetRenderDrawColor(renderer, buttonBorder.r, buttonBorder.g, buttonBorder.b, 255);
	SDL_RenderFillRect(renderer, &closeRectBorder);

	SDL_SetRenderDrawColor(renderer, buttonInside.r, buttonInside.g, buttonInside.b, 255);
	SDL_RenderFillRect(renderer, &closeRect);


}

bool MenuManager::openShop()
{
	if (m_currentMenu == Menus::none) {
		m_currentMenu = Menus::shop;
		return true;
	}
	return false;
}
