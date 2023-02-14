#include "MenuManager.h"
#include "Resources.h"
#include "SDL_image.h"
#include "Interface.h"
#include "World.h"
#include "Enemy.h"
#include "Player.h"
#include "TradingPost.h"
#include <math.h>
#include <string>


MenuManager::MenuManager(SDL_Renderer* renderer, GameHandler* m_p_gameHandler, World* m_p_currenWorld)
{
	m_currentMenu = Menus::start;
	this->m_p_gameHandler = m_p_gameHandler;
	this->m_p_currenWorld = m_p_currenWorld;

	SDL_Surface* p_tmpSurface = IMG_Load(RSC_SHOP_MENU);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_GAME_OVER);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_START_LOGO);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_START_TEXT);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	m_menuOpacity = 0;
	SDL_SetTextureAlphaMod(m_menuTextures[1], m_menuOpacity);	//Texture is now transparent (so we can make an blend effect later on)
}

MenuManager::~MenuManager()
{
	int numberOfElements = int(m_menuTextures.size());
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_menuTextures.back());
		m_menuTextures.pop_back();
	}
}

gameStates MenuManager::interactWithMenu(bool mouseButtonPressed, SDL_Renderer* renderer, double deltaTime)
{
	switch (m_currentMenu) {
		case Menus::none:
			break;
		case Menus::shop:
			renderShop(mouseButtonPressed, renderer);
			break;
		case Menus::gameOver:
			if (renderGameOver(renderer, deltaTime))
				return gameStates::hasEnded;
			break;
		case Menus::start:
			renderStartMenu(mouseButtonPressed, renderer, deltaTime);
			return gameStates::isStarting;
	}
	return gameStates::isRunning;
}

bool MenuManager::renderGameOver(SDL_Renderer* renderer, double deltaTime)
{
	m_menuOpacity += short(deltaTime / 2.0);

	if (m_menuOpacity >= 255) {
		m_menuOpacity = 255;
	}
	SDL_SetTextureAlphaMod(m_menuTextures[1], m_menuOpacity);
	SDL_RenderCopy(renderer, m_menuTextures[1], NULL, NULL);


	std::string displayText = std::to_string(m_p_gameHandler->getWaveCounter());
	SDL_Surface* surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(75), displayText.c_str(), {38, 38, 44});
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
	SDL_SetTextureAlphaMod(textureText, m_menuOpacity);

	SDL_Rect textRect = {400, 370, surfaceText->w, surfaceText->h };
	textRect.x -= surfaceText->w / 2;
	textRect.y -= surfaceText->h / 2;

	SDL_RenderCopy(renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);

	return (m_menuOpacity == 255);
}

void MenuManager::renderShop(bool mouseButtonPressed, SDL_Renderer* renderer)
{
	int mousePosX, mousePosY;
	float renderScale = Interface::getInstance().getPixelPerPixel();
	SDL_GetMouseState(&mousePosX, &mousePosY);
	mousePosX /= renderScale;
	mousePosY /= renderScale;
	SDL_Rect mouseBounds = { mousePosX, mousePosY, 5, 5 };
	SDL_RenderCopy(renderer, m_menuTextures[0], NULL, NULL);

	//----------------------------------------------------------------------------------------------- Render "close" and "next wave" buttons

	SDL_Color buttonBorder = { 229, 229, 203 };
	SDL_Color buttonInside = { 27, 18, 15 };
	SDL_Rect buttonRect = { 82, 127, 175, 45 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			m_currentMenu = Menus::none;
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(30), "Close Shop", buttonInside, buttonBorder, renderer);

	buttonRect = { 800 - 82 - 175, 127, 175, 45 };
	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			m_currentMenu = Menus::none;
			m_p_currenWorld->sendMerchantAway();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(30), "Next Wave", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "buy health potion" button

	TradingPost* p_merchant = m_p_currenWorld->getMerchant();
	int* upgradesCounter = p_merchant->getUpgrade1Sold();
	int price = 30 + upgradesCounter[0] * 30;
	std::string displayText = std::to_string(price) + "C";

	buttonBorder = { 255, 191, 0 };
	buttonInside = { 255, 112, 0 };
	buttonRect = { 98, 435, 145, 45 };
	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 255, 201, 10 };
		buttonInside = { 235, 92, 0 };
		if (mouseButtonPressed) {			//Button pressed
			buyHealthPotion(upgradesCounter, price);
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(30), displayText, buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "buy another heart" button

	upgradesCounter = p_merchant->getUpgrade2Sold();
	price = 40 + upgradesCounter[0] * 40;
	displayText = std::to_string(price) + "C";

	buttonBorder = { 255, 191, 0 };
	buttonInside = { 255, 112, 0 };
	buttonRect = { 328, 435, 145, 45 };
	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 255, 201, 10 };
		buttonInside = { 235, 92, 0 };
		if (mouseButtonPressed) {			//Button pressed
			buyMoreHealth(upgradesCounter, price);
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(30), displayText, buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "increase stamina" button

	upgradesCounter = p_merchant->getUpgrade3Sold();
	price = 60 + upgradesCounter[0] * 60;
	displayText = (upgradesCounter[0] < 15) ? std::to_string(price) + "C" : "Max";

	buttonBorder = { 255, 191, 0 };
	buttonInside = { 255, 112, 0 };
	buttonRect = { 557, 435, 145, 45 };
	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 255, 201, 10 };
		buttonInside = { 235, 92, 0 };
		if (mouseButtonPressed) {			//Button pressed
			buyMoreStamina(upgradesCounter, price);
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(30), displayText, buttonInside, buttonBorder, renderer);

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
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;

	SDL_RenderCopy(renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);
}

void MenuManager::renderStartMenu(bool mouseButtonPressed, SDL_Renderer* renderer, double deltaTime)
{
	SDL_FRect startMenuRect = { 50, -80, 800, 640 };
	startMenuRect.y += 50.0f * sin(SDL_GetTicks() / 1000.0f);		//Formula for an oscillation
	SDL_RenderCopyF(renderer, m_menuTextures[2], NULL, &startMenuRect);

	startMenuRect = { 179, 520, 442, 39 };
	startMenuRect.y += 5.0f * sin(SDL_GetTicks() / 200.0f);
	SDL_RenderCopyF(renderer, m_menuTextures[3], NULL, &startMenuRect);

	return;
}

void MenuManager::buyHealthPotion(int* itemBoughtCounter, int price)
{
	Player* p_player = m_p_currenWorld->getPlayer();
	if (p_player->getCoinCounter() < price || p_player->getCurrentLives() == p_player->getMaxLives())
		return;
	p_player->updateCoinCounter(price * (-1));
	p_player->healBody();
	(*itemBoughtCounter)++;	//Items has been bought one more time
}

void MenuManager::buyMoreHealth(int* itemBoughtCounter, int price)
{
	Player* p_player = m_p_currenWorld->getPlayer();
	if (p_player->getCoinCounter() < price)
		return;
	p_player->updateCoinCounter(price * (-1));
	p_player->updateMaxLives(1);
	(*itemBoughtCounter)++;	//Items has been bought one more time
}

void MenuManager::buyMoreStamina(int* itemBoughtCounter, int price)
{
	Player* p_player = m_p_currenWorld->getPlayer();
	if (p_player->getCoinCounter() < price || *itemBoughtCounter > 15)	//Item cant be bought more than 16 times
		return;
	p_player->updateCoinCounter(price * (-1));
	p_player->updateAttackCooldown(-250);
	(*itemBoughtCounter)++;	//Items has been bought one more time
}

bool MenuManager::openShop()
{
	if (m_currentMenu == Menus::none) {
		m_currentMenu = Menus::shop;
		return true;
	}
	return false;
}
