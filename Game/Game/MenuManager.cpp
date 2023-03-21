#include "MenuManager.h"
#include "Resources.h"
#include "SDL_image.h"
#include "Interface.h"
#include "World.h"
#include "Enemy.h"
#include "Player.h"
#include "TradingPost.h"
#include "SoundHandler.h"
#include <math.h>
#include <string>
#include <fstream>

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

	p_tmpSurface = IMG_Load(RSC_PAUSE_MENU);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_OPTION_BUTTON);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_OPTIONS_BACKGROUND);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_AUDIO_ICONS);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_IP_INPUT);
	m_menuTextures.push_back(SDL_CreateTextureFromSurface(renderer, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	m_menuOpacity = 0;
	SDL_SetTextureAlphaMod(m_menuTextures[1], m_menuOpacity);	//Texture is now transparent (so we can make an blend effect later on)

	m_ip4vAddress = "UNKNOWN";
	findOutLocalIpAddress();
}

MenuManager::~MenuManager()
{
	int numberOfElements = int(m_menuTextures.size());
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_menuTextures.back());
		m_menuTextures.pop_back();
	}
}

void MenuManager::generateUserInput(SDL_Renderer* renderer)
{

	SDL_Rect inputBoxRect = { 316, 355, 400, 60 };
	SDL_Rect textRect = { 471, 360, 240, 50 };
	SDL_Event* p_inputQueue = Interface::getInstance().getInputQueue();
	Uint32 currentTime = SDL_GetTicks(); //Calculate delta time
	Uint32 lastTime = currentTime;

	char inputedNumber = 0;	//No Input

	while (true)
	{
		currentTime = SDL_GetTicks();
		float deltaTime = float(currentTime - lastTime);

		if (deltaTime >= 1000.0f / 60.0f) //Limit FPS auf 60
		{

			lastTime = currentTime;
			while (SDL_PollEvent(Interface::getInstance().getInputQueue()) != 0)
			{

				switch (Interface::getInstance().getInputQueue()->type)
				{
				case SDL_QUIT:
					exit(1);		//Terminate the programm (It is safe to assume that there are no other clients connected)
					break;

				case SDL_KEYDOWN:
					switch (Interface::getInstance().getInputQueue()->key.keysym.sym)
					{

					case SDLK_1:
						inputedNumber = '1';
						break;

					case SDLK_2:
						inputedNumber = '2';
						break;

					case SDLK_3:
						inputedNumber = '3';
						break;

					case SDLK_4:
						inputedNumber = '4';
						break;

					case SDLK_5:
						inputedNumber = '5';
						break;

					case SDLK_6:
						inputedNumber = '6';
						break;

					case SDLK_7:
						inputedNumber = '7';
						break;

					case SDLK_8:
						inputedNumber = '8';
						break;

					case SDLK_9:
						inputedNumber = '9';
						break;

					case SDLK_0:
						inputedNumber = '0';
						break;

					case SDLK_PERIOD:
						inputedNumber = '.';
						break;

					case SDLK_BACKSPACE:
						inputedNumber = '\b';
						break;

					case SDLK_KP_ENTER:
					case SDLK_RETURN:
					case SDLK_ESCAPE:
						return;


					}
					break;

				}

			}

			if (inputedNumber == 0)	//No user input yet
				continue;

			if (inputedNumber == '\b') {	//Removes last character from string
				if (m_userInput.length()) {	//Only if the string contains any letters
					m_userInput.pop_back();
					inputedNumber = 0;
				}
				continue;
			}

			if (m_userInput.length() > 14)	//Cant write an infinite IP (This would break the GUI)
				continue;

			m_userInput.push_back(inputedNumber);
			inputedNumber = 0;
		}
		//std::cout << m_userInput << std::endl;

		SDL_RenderCopy(renderer, m_menuTextures[8], NULL, &inputBoxRect);
		SDL_Color textColor = { 27, 18, 15 };

		if (m_userInput.length()) {

			SDL_Surface* surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(Fonts::eightBit, 30), m_userInput.c_str(), textColor);
			SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

			textRect.x += textRect.w / 2 - surfaceText->w / 2;
			textRect.y += textRect.h / 2 - surfaceText->h / 2;
			textRect.w = surfaceText->w;
			textRect.h = surfaceText->h;
			SDL_RenderCopy(renderer, textureText, NULL, &textRect);
			SDL_FreeSurface(surfaceText);
			SDL_DestroyTexture(textureText);
		}

		if ((SDL_GetTicks() / 1000) % 2 == 0 && m_userInput.length() <= 14) {	//Only every other second and when you can still add charcters
			SDL_SetRenderDrawColor(renderer, textColor.r, textColor.g, textColor.b, 255);
			SDL_Rect pipeRect = { (m_userInput.length()) ? textRect.x + textRect.w + 3 : textRect.x + 3, 364, 3, 42 };

			SDL_RenderFillRect(renderer, &pipeRect);
		}



		SDL_RenderPresent(renderer);

	}
}

bool MenuManager::tryClosingMenu()
{
	switch (m_currentMenu) {
	case Menus::none:
	case Menus::pause:
	case Menus::shop:
	case Menus::start:
		m_currentMenu = Menus::none;
		return true;

	case Menus::optionsDefault:	//These menus will only open another menu back up when closed
	case Menus::optionsMultiplayer:
		m_currentMenu = Menus::start;
		return true;

	default:
		return false;
	}
}

GameStates MenuManager::interactWithMenu(bool mouseButtonPressed, SDL_Renderer* renderer, double deltaTime)
{
	switch (m_currentMenu) {
		case Menus::none:
			break;

		case Menus::shop:
			renderShop(mouseButtonPressed, renderer);
			break;

		case Menus::gameOver:
			if (renderGameOver(renderer, deltaTime))
				return GameStates::hasEnded;
			break;

		case Menus::start:
			renderStartMenu(mouseButtonPressed, renderer, deltaTime);
			return GameStates::isStarting;

		case Menus::pause:
			if (renderPauseMenu(mouseButtonPressed, renderer))
				return GameStates::hasEnded;
			break;
		case Menus::optionsDefault:
		case Menus::optionsMultiplayer:
			renderOptionsMenu(mouseButtonPressed, renderer);
			return GameStates::isStarting;
			break;
			
	}
	return GameStates::isRunning;
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
	SDL_Surface* surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(Fonts::eightBit, 75), displayText.c_str(), {38, 38, 44});
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
	SDL_SetTextureAlphaMod(textureText, m_menuOpacity);

	SDL_Rect textRect = {400, 370, surfaceText->w, surfaceText->h };
	textRect.x -= surfaceText->w / 2;
	textRect.y -= surfaceText->h / 2;

	SDL_RenderCopy(renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);
	if (m_menuOpacity == 255) {			//Animation is done playing, i render the menu one last time and wait for input before returning true and reseting the game
		SDL_RenderPresent(renderer);
		Interface::getInstance().waitForInput(300);
		SoundHandler::getInstance().playClickSound();
		return true;
	}
	return false;
}

void MenuManager::renderShop(bool mouseButtonPressed, SDL_Renderer* renderer)
{
	int mousePosX, mousePosY;
	float renderScale = Interface::getInstance().getPixelPerPixel();
	SDL_GetMouseState(&mousePosX, &mousePosY);
	mousePosX /= renderScale;
	mousePosY /= renderScale;
	SDL_Rect mouseBounds = { mousePosX, mousePosY, 1, 1 };
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
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 30), "Close Shop", buttonInside, buttonBorder, renderer);

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
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 30), "Next Wave", buttonInside, buttonBorder, renderer);

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
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 30), displayText, buttonInside, buttonBorder, renderer);

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
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 30), displayText, buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "increase stamina" button

	upgradesCounter = p_merchant->getUpgrade3Sold();
	price = 60 + upgradesCounter[0] * 60;
	displayText = (upgradesCounter[0] <= 15) ? std::to_string(price) + "C" : "Max";

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
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 30), displayText, buttonInside, buttonBorder, renderer);

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

	//---------------------------------------------------------------------------------------------- Text on the button

	SDL_Surface* surfaceText = TTF_RenderText_Solid(font, displayText.c_str(), borderColor);
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

void MenuManager::renderButton(SDL_Rect buttonBounds, TTF_Font* font, std::string displayText, SDL_Color buttonColor, SDL_Color borderColor, SDL_Color fontColor, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 255);
	SDL_RenderFillRect(renderer, &buttonBounds);

	buttonBounds.x += 5;
	buttonBounds.y += 5;
	buttonBounds.w -= 10;
	buttonBounds.h -= 10;

	SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, 255);
	SDL_RenderFillRect(renderer, &buttonBounds);

	//---------------------------------------------------------------------------------------------- Text on the button

	SDL_Surface* surfaceText = TTF_RenderText_Solid(font, displayText.c_str(), fontColor);
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
	int mousePosX, mousePosY;
	float renderScale = Interface::getInstance().getPixelPerPixel();
	SDL_GetMouseState(&mousePosX, &mousePosY);
	mousePosX /= renderScale;
	mousePosY /= renderScale;
	SDL_Rect mouseBounds = { mousePosX, mousePosY, 1, 1 };

	SDL_FRect startMenuRect = { 50, -80, 800, 640 };
	startMenuRect.y += 50.0f * sin(SDL_GetTicks() / 1000.0f);		//Formula for an oscillation
	SDL_RenderCopyF(renderer, m_menuTextures[2], NULL, &startMenuRect);

	startMenuRect = { 179, 520, 442, 39 };
	startMenuRect.y += 5.0f * sin(SDL_GetTicks() / 200.0f);
	SDL_RenderCopyF(renderer, m_menuTextures[3], NULL, &startMenuRect);

	SDL_Rect optionsButtonRect = { 715, 8, 77, 76 };	//The options button gets rendered
	SDL_RenderCopy(renderer, m_menuTextures[5], NULL, &optionsButtonRect);

	if (SDL_HasIntersection(&mouseBounds, &optionsButtonRect)&& mouseButtonPressed) {	//Mouse clicked on the button
		m_currentMenu = Menus::optionsDefault;
		SoundHandler::getInstance().playClickSound();
	}
	return;
}

bool MenuManager::renderPauseMenu(bool mouseButtonPressed, SDL_Renderer* renderer)
{
	bool returnValue = false;
	int mousePosX, mousePosY;
	float renderScale = Interface::getInstance().getPixelPerPixel();
	SDL_GetMouseState(&mousePosX, &mousePosY);
	mousePosX /= renderScale;
	mousePosY /= renderScale;
	SDL_Rect mouseBounds = { mousePosX, mousePosY, 1, 1 };

	SDL_RenderCopy(renderer, m_menuTextures[4], NULL, NULL);


	SDL_Rect iconsRect = {356, 266, 88, 237};
	SDL_RenderCopy(renderer, m_menuTextures[7], NULL, &iconsRect);

	//----------------------------------------------------------------------------------------------- Render "close" button

	SDL_Color buttonBorder = { 229, 229, 203 };
	SDL_Color buttonInside = { 27, 18, 15 };
	SDL_Rect buttonRect = { 245, 190, 150, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			m_currentMenu = Menus::none;
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 34), "Resume", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "restart" button

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	buttonRect = { 255 + 150, 190, 150, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			m_currentMenu = Menus::none;
			SoundHandler::getInstance().playClickSound();
			returnValue = true;
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 34), "Restart", buttonInside, buttonBorder, renderer);


	//----------------------------------------------------------------------------------------------- Render "audio +" button

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	buttonRect = { 472, 292, 64, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			SoundHandler::getInstance().updateAudioVolume(1);
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::pixelSplitter, 40), "+", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "audio -" button

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	buttonRect = { 264, 292, 64, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			SoundHandler::getInstance().updateAudioVolume(-1);
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::pixelSplitter, 40), "-", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "music +" button

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	buttonRect = { 472, 413, 64, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			SoundHandler::getInstance().updateMusicVolume(1);
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::pixelSplitter, 40), "+", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "music -" button

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	buttonRect = { 264, 413, 64, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			SoundHandler::getInstance().updateMusicVolume(-1);
			SoundHandler::getInstance().playClickSound();

		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::pixelSplitter, 40), "-", buttonInside, buttonBorder, renderer);



	//---------------------------------------------------------------------------------------------- Displaying volume
	std::string displayText = std::to_string(SoundHandler::getInstance().getAudioVolumePercent());
	SDL_Color textColor = { 27, 18, 15 };

	SDL_Surface* surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(Fonts::eightBit, 30), displayText.c_str(), textColor);
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

	SDL_Rect textRect = { 361, 273, 78, 32 };
	textRect.x += textRect.w / 2 - surfaceText->w / 2;
	textRect.y += textRect.h / 2 - surfaceText->h / 2;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;
	SDL_RenderCopy(renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);

	displayText = std::to_string(SoundHandler::getInstance().getMusicVolumePercent());
	textColor = { 27, 18, 15 };

	surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(Fonts::eightBit, 30), displayText.c_str(), textColor);
	textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

	textRect = { 361, 394, 78, 32 };
	textRect.x += textRect.w / 2 - surfaceText->w / 2;
	textRect.y += textRect.h / 2 - surfaceText->h / 2;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;
	SDL_RenderCopy(renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);

	return returnValue;
}

void MenuManager::renderOptionsMenu(bool mouseButtonPressed, SDL_Renderer* renderer)
{
	int mousePosX, mousePosY;
	float renderScale = Interface::getInstance().getPixelPerPixel();
	SDL_GetMouseState(&mousePosX, &mousePosY);
	mousePosX /= renderScale;
	mousePosY /= renderScale;
	SDL_Rect mouseBounds = { mousePosX, mousePosY, 1, 1 };

	SDL_RenderCopy(renderer, m_menuTextures[6], NULL, NULL);

	//----------------------------------------------------------------------------------------------- Render "Audio" button

	SDL_Color buttonBorder = { 77, 51, 38 };
	SDL_Color buttonInside = { 77, 51, 38 };
	SDL_Color fontColor = { 26, 18, 11 };
	SDL_Rect buttonRect = { 53, 53, 231, 60 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 66, 44, 34 };
		buttonInside = { 66, 44, 34 };
		if (mouseButtonPressed) {			//Button pressed
			m_currentMenu = Menus::optionsDefault;
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 34), "Audio", buttonInside, buttonBorder, fontColor, renderer);

	//------------------------------------------------------------------------------------------------ Render "Multiplayer" button
	
	buttonRect.y += 60;
	buttonBorder = { 77, 51, 38 };
	buttonInside = { 77, 51, 38 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 66, 44, 34 };
		buttonInside = { 66, 44, 34 };
		if (mouseButtonPressed) {			//Button pressed
			m_currentMenu = Menus::optionsMultiplayer;
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 34), "Multiplayer", buttonInside, buttonBorder, fontColor, renderer);

	//------------------------------------------------------------------------------------------------ Render the rest of the current options menu

	switch (m_currentMenu) {
	case Menus::optionsDefault:
		renderOptionsAudio(mouseButtonPressed, renderer);
		break;
	case Menus::optionsMultiplayer:
		renderOptionsMultiplayer(mouseButtonPressed, renderer);
		break;
	}

}

void MenuManager::renderOptionsAudio(bool mouseButtonPressed, SDL_Renderer* renderer)
{
	//INFO: Removing the -119 offset will center the icons

	bool returnValue = false;
	int mousePosX, mousePosY;
	float renderScale = Interface::getInstance().getPixelPerPixel();
	SDL_GetMouseState(&mousePosX, &mousePosY);
	mousePosX /= renderScale;
	mousePosY /= renderScale;
	SDL_Rect mouseBounds = { mousePosX, mousePosY, 1, 1 };

	SDL_Rect iconsRect = { 473, 202 - 119,  88, 237 };
	SDL_RenderCopy(renderer, m_menuTextures[7], NULL, &iconsRect);

	//----------------------------------------------------------------------------------------------- Render "audio +" button

	SDL_Color buttonBorder = { 229, 229, 203 };
	SDL_Color buttonInside = { 27, 18, 15 };
	SDL_Rect buttonRect = { 589, 228 - 119, 64, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			SoundHandler::getInstance().updateAudioVolume(1);
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::pixelSplitter, 40), "+", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "audio -" button

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	buttonRect = { 381, 228 - 119, 64, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			SoundHandler::getInstance().updateAudioVolume(-1);
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::pixelSplitter, 40), "-", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "music +" button

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	buttonRect = { 589, 349 - 119, 64, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			SoundHandler::getInstance().updateMusicVolume(1);
			SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::pixelSplitter, 40), "+", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------- Render "music -" button

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	buttonRect = { 381, 349 - 119, 64, 64 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			SoundHandler::getInstance().updateMusicVolume(-1);
			SoundHandler::getInstance().playClickSound();

		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::pixelSplitter, 40), "-", buttonInside, buttonBorder, renderer);


	//---------------------------------------------------------------------------------------------- Displaying volume
	std::string displayText = std::to_string(SoundHandler::getInstance().getAudioVolumePercent());
	SDL_Color textColor = { 27, 18, 15 };

	SDL_Surface* surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(Fonts::eightBit, 30), displayText.c_str(), textColor);
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

	SDL_Rect textRect = { 478, 209 - 119, 78, 32 };
	textRect.x += textRect.w / 2 - surfaceText->w / 2;
	textRect.y += textRect.h / 2 - surfaceText->h / 2;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;
	SDL_RenderCopy(renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);

	displayText = std::to_string(SoundHandler::getInstance().getMusicVolumePercent());
	textColor = { 27, 18, 15 };

	surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(Fonts::eightBit, 30), displayText.c_str(), textColor);
	textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

	textRect = { 478, 330 - 119, 78, 32 };
	textRect.x += textRect.w / 2 - surfaceText->w / 2;
	textRect.y += textRect.h / 2 - surfaceText->h / 2;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;
	SDL_RenderCopy(renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);

}

void MenuManager::renderOptionsMultiplayer(bool mouseButtonPressed, SDL_Renderer* renderer)
{
	bool returnValue = false;
	int mousePosX, mousePosY;
	float renderScale = Interface::getInstance().getPixelPerPixel();
	SDL_GetMouseState(&mousePosX, &mousePosY);
	mousePosX /= renderScale;
	mousePosY /= renderScale;
	SDL_Rect mouseBounds = { mousePosX, mousePosY, 1, 1 };

	//---------------------------------------------------------------------------------------------- Displaying your ip
	std::string displayText = "Your IP is " + m_ip4vAddress;
	SDL_Color textColor = { 27, 18, 15 };

	SDL_Surface* surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(Fonts::eightBit, 30), displayText.c_str(), textColor);
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

	SDL_Rect textRect = { 316, 83, 400, 60 };
	textRect.x += textRect.w / 2 - surfaceText->w / 2;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;
	SDL_RenderCopy(renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);

	//----------------------------------------------------------------------------------------------  Render "Start server" button

	SDL_Color buttonBorder = { 229, 229, 203 };
	SDL_Color buttonInside = { 27, 18, 15 };
	SDL_Rect buttonRect = { 316, 83 + 60, 400, 60 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			if(m_p_gameHandler->initiateServer())
				SoundHandler::getInstance().playClickSound();
			
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 30), (m_p_gameHandler->getGameHandlerType() == GameHandlerType::server) ? "Waiting for clients..." : "Start server", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------  Render "Join server" button

	buttonBorder = { 229, 229, 203 };
	buttonInside = { 27, 18, 15 };
	buttonRect = { 316, 83 + 180, 400, 60 };

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		buttonBorder = { 240, 240, 240 };
		buttonInside = { 20, 0, 0 };
		if (mouseButtonPressed) {			//Button pressed
			if(m_p_gameHandler->initiateClient(/*"127.0.0.1"*/m_userInput))
				SoundHandler::getInstance().playClickSound();
		}
	}
	renderButton(buttonRect, m_p_gameHandler->getFont(Fonts::eightBit, 30), (m_p_gameHandler->getGameHandlerType() == GameHandlerType::client) ? "Joining server..." : "Join server", buttonInside, buttonBorder, renderer);

	//----------------------------------------------------------------------------------------------  Render IP input box

	buttonRect.y += 92;
	SDL_RenderCopy(renderer, m_menuTextures[8], NULL, &buttonRect);

	if (SDL_HasIntersection(&mouseBounds, &buttonRect)) {	//Mouse hovered over button
		if (mouseButtonPressed && m_p_gameHandler->getGameHandlerType() == GameHandlerType::singleplayer) {			//Button pressed and the game is currently offline
			generateUserInput(renderer);
		}
	}

	//---------------------------------------------------------------------------------------------- Render the Inputed IP

	if (m_userInput.length()) {
		textRect = { 471, 360, 240, 50 };
		surfaceText = TTF_RenderText_Solid(m_p_gameHandler->getFont(Fonts::eightBit, 30), m_userInput.c_str(), textColor);
		textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

		textRect.x += textRect.w / 2 - surfaceText->w / 2;
		textRect.y += textRect.h / 2 - surfaceText->h / 2;
		textRect.w = surfaceText->w;
		textRect.h = surfaceText->h;
		SDL_RenderCopy(renderer, textureText, NULL, &textRect);
		SDL_FreeSurface(surfaceText);
		SDL_DestroyTexture(textureText);
	}

}

void MenuManager::buyHealthPotion(int* itemBoughtCounter, int price)
{
	Player* p_player = m_p_currenWorld->getPlayer();
	if (p_player->getCoinCounter() < price || p_player->getCurrentLives() == p_player->getMaxLives())
		return;
	SoundHandler::getInstance().playClickSound();
	p_player->updateCoinCounter(price * (-1));
	p_player->healBody();
	(*itemBoughtCounter)++;	//Items has been bought one more time
}

void MenuManager::buyMoreHealth(int* itemBoughtCounter, int price)
{
	Player* p_player = m_p_currenWorld->getPlayer();
	if (p_player->getCoinCounter() < price)
		return;

	SoundHandler::getInstance().playClickSound();
	p_player->updateCoinCounter(price * (-1));
	p_player->updateMaxLives(1);
	(*itemBoughtCounter)++;	//Items has been bought one more time
}

void MenuManager::buyMoreStamina(int* itemBoughtCounter, int price)
{
	Player* p_player = m_p_currenWorld->getPlayer();
	if (p_player->getCoinCounter() < price || *itemBoughtCounter > 15)	//Item cant be bought more than 16 times
		return;

	SoundHandler::getInstance().playClickSound();
	p_player->updateCoinCounter(price * (-1));
	p_player->updateAttackCooldown(-250);
	(*itemBoughtCounter)++;	//Items has been bought one more time
}

void MenuManager::findOutLocalIpAddress()
{
	std::string line;
	std::ifstream IPFile;
	std::string searchQuery = "IPv4-Adresse  . . . . . . . . . . :";

	system("ipconfig > ip.txt");
	IPFile.open("ip.txt");

	if (IPFile.is_open())
	{
		while (!IPFile.eof()) {

			getline(IPFile, line);
			if (line.find(searchQuery) != std::string::npos) {
				line.erase(0, 39);
				m_ip4vAddress = line;
				break;
			}
		}

		IPFile.close();
	}
	else {
		std::cout << "Error, can't find ip adress\n";
	}
}

bool MenuManager::openMenu(Menus newMenu)
{
	if (m_currentMenu == newMenu)
		return true;

	if (m_currentMenu == Menus::none || newMenu == Menus::gameOver) {
		m_currentMenu = newMenu;
		return true;
	}

	return false;
}