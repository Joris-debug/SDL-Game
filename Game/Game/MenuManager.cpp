#include "MenuManager.h"
#include "Resources.h"
#include "SDL_image.h"
#include "World.h"
#include "Enemy.h"

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
	SDL_Rect closeRect = { 82, 127, 175, 45 };
	SDL_Rect nextWaveRect = { 800 - 82 - 175, 127, 175, 45 };

	if (SDL_HasIntersection(&mouseBounds, &closeRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			m_currentMenu = Menus::none;
		}
	}
	renderButton(closeRect, m_p_gameHandler->getFont(30), "Close Shop", buttonInside, buttonBorder, renderer);

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	if (SDL_HasIntersection(&mouseBounds, &nextWaveRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			m_currentMenu = Menus::none;
			m_p_currenWorld->sendMerchantAway();
		}
	}
	renderButton(nextWaveRect, m_p_gameHandler->getFont(30), "Next Wave", buttonInside, buttonBorder, renderer);
}

void MenuManager::renderButton(SDL_Rect buttonBounds, TTF_Font* font, std::string displayText, SDL_Color buttonColor, SDL_Color borderColor, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 255);
	SDL_RenderFillRect(renderer, &buttonBounds);

	buttonBounds.x += 5;
	buttonBounds.y += 5;
	buttonBounds.w -= 10;
	buttonBounds.h -= 10;

	SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, 255);
	SDL_RenderFillRect(renderer, &buttonBounds);

	//---------------------------------------------------------------------------------------------- Text on the close button

	SDL_Surface* surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(30), displayText.c_str(), borderColor);
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

	SDL_Rect textRect = buttonBounds;
	textRect.x += textRect.w / 2 - surfaceText->w / 2;
	textRect.y += textRect.h / 2 - surfaceText->h / 2;
	textRect.y += 2;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;

	SDL_RenderCopy(renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);
}

bool MenuManager::openShop()
{
	if (m_currentMenu == Menus::none) {
		m_currentMenu = Menus::shop;
		return true;
	}
	return false;
}
