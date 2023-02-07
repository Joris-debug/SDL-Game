#include "GameHandler.h"
#include "Interface.h"
#include "World.h"
#include "Resources.h"
#include "Enemy.h"
#include "Player.h"
#include "SDL_image.h"
#include "Effect.h"
#include "MenuManager.h"
#include <string>
int GameHandler::gameLoop()
{
	int x_input = 0, y_input = 0;
	bool leftMouseButtonPressed = false, eKeyPressed = false, fKeyPressed = false;
	Uint32 currentTime = SDL_GetTicks(); //Calculate delta time
	Uint32 lastTime = currentTime;
	while (true)
	{
		currentTime = SDL_GetTicks();
		m_deltaTime = float(currentTime - lastTime);

		if (m_deltaTime > float(1000 / 60)) //Limit FPS auf 60
		{
			lastTime = currentTime;
			while (SDL_PollEvent(m_p_interface->getInputQueue()) != 0)
			{

				switch (m_p_interface->getInputQueue()->type)
				{
				case SDL_QUIT:
					exit(1);
					break;

				case SDL_MOUSEBUTTONDOWN:
					leftMouseButtonPressed = true;
					break;

				case SDL_KEYDOWN:
					switch (m_p_interface->getInputQueue()->key.keysym.sym)
					{

					case SDLK_w:
						y_input = 1;
						break;

					case SDLK_s:
						y_input = -1;
						break;

					case SDLK_a:
						x_input = 1;
						break;

					case SDLK_d:
						x_input = -1;
						break;

					case SDLK_e:
						eKeyPressed = true;
						break;

					case SDLK_f:
						fKeyPressed = true;
						break;
					}
					break;

				case SDL_KEYUP:

					switch (m_p_interface->getInputQueue()->key.keysym.sym)
					{
					case SDLK_w:
					case SDLK_s:
						y_input = 0;
						break;

					case SDLK_d:
					case SDLK_a:
						x_input = 0;
						break;
					}
					break;

				}
			}
			checkCurrentWave();

			if (!MenuManager::getInstance().checkIfMenuOpen()) {		//No window is open

				if (leftMouseButtonPressed) {
					m_p_currentWorld->triggerPlayerAttack();
				}
				if (eKeyPressed) {
					m_p_currentWorld->talkToMerchant();
				}
			}
			else {	//Player doesnt move when a window is open
				x_input = 0;
				y_input = 0;
			}

			m_p_interface->getPixelPerPixel();
			m_p_currentWorld->moveWorld(x_input, y_input, 0.2f * m_deltaTime );

			renderEverything(leftMouseButtonPressed);
			leftMouseButtonPressed = false;
			eKeyPressed = false;

			if (!m_p_currentWorld->getPlayer()->getCurrentLives()) {		//Player is GameOver
				break;
			}
		}

	}

	m_p_interface->waitForInput(1000);
	return 0;
}

GameHandler::GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_)
{
	this->m_p_interface = m_p_interface_;
	this->m_p_renderer = m_p_renderer_;
	this->m_deltaTime = 1;
	m_waveCounter = 0;
	m_waveTimer = 0;
	m_p_waveClock = new Clock(1000);
	m_p_currentWorld = nullptr;
	MenuManager::getInstance().createTextures(m_p_renderer);

	SDL_Surface* p_tmpSurface;

	//Load all Mantis Spritesheets
	p_tmpSurface = IMG_Load(RSC_MANTIS_IDLE);
	m_enemyTexturesIdle.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MANTIS_WALK);
	m_enemyTexturesWalk.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));	
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MANTIS_HIT);
	m_enemyTexturesHit.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//Load all Maggot Spritesheets
	p_tmpSurface = IMG_Load(RSC_MAGGOT_IDLE);
	m_enemyTexturesIdle.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MAGGOT_WALK);
	m_enemyTexturesWalk.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MAGGOT_HIT);
	m_enemyTexturesHit.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//Load all hud textures
	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_BORDER);
	m_hudTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_LIVES);
	m_hudTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_STAMINA);
	m_hudTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_CHECK);
	m_hudTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_WOODEN_BOARD_RIGHT);
	m_hudTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_WOODEN_BOARD_LEFT);
	m_hudTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_BOTTOM_HUD);
	m_hudTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_ENEMYBAR);
	m_hudTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_ENEMYBAR_BORDER);
	m_hudTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//---------------------------------------------------------------------------------- Load all effect textures
	p_tmpSurface = IMG_Load(RSC_EFFECT_EXPLOSION1);
	m_effectTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//---------------------------------------------------------------------------------- Load misc textures
	p_tmpSurface = IMG_Load(RSC_WORLD_BACKGROUND);
	m_miscTextures.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//----------------------------------------------------------------------------------- Load all fonts
	m_gameFonts.push_back(TTF_OpenFont(RSC_8BIT_FONT, 45));	//Font used for the wave counter
	m_gameFonts.push_back(TTF_OpenFont(RSC_8BIT_FONT, 30));	//Font used for the enemy counter
	m_gameFonts.push_back(TTF_OpenFont(RSC_8BIT_FONT, 34));	//Font used for wave timer + coin counter
}

GameHandler::~GameHandler()
{
	delete m_p_waveClock;
	delete m_p_currentWorld;
	MenuManager::getInstance().deleteTextures();

	int numberOfElements = int(m_enemyTexturesIdle.size());
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_enemyTexturesIdle.back());
		m_enemyTexturesIdle.pop_back();
		SDL_DestroyTexture(m_enemyTexturesWalk.back());
		m_enemyTexturesWalk.pop_back();
		SDL_DestroyTexture(m_enemyTexturesHit.back());
		m_enemyTexturesHit.pop_back();
	}

	numberOfElements = int(m_hudTextures.size());
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_hudTextures.back());
		m_hudTextures.pop_back();
	}

	numberOfElements = int(m_effectTextures.size());
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_effectTextures.back());
		m_effectTextures.pop_back();
	}

	numberOfElements = int(m_miscTextures.size());
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_miscTextures.back());
		m_miscTextures.pop_back();
	}

	//Delete all fonts
	numberOfElements = int(m_gameFonts.size());
	for (int i = 0; i < numberOfElements; i++) {
		TTF_CloseFont(m_gameFonts.back());
		m_gameFonts.pop_back();
	}
	
}

int GameHandler::initWorld()
{
	m_p_currentWorld = new World(IMG_Load(RSC_LEVEL_1), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer, &m_randomNumberEngine, new Effect({0, 0, 256, 160}, m_effectTextures[0]));
	m_p_currentWorld->addVinicityToMap(new Vicinity(IMG_Load(RSC_LEVEL_1_TOP), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer));

	m_p_currentWorld->addEntityToMap(new Entity({ -1232, -1280, 3264, 32 })); // Border left
	m_p_currentWorld->addEntityToMap(new Entity({ -1232, -1280 + 1616*2, 3264, 32 })); // Border bottom
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1616 * 2, -1280, 32, 3264 })); // Border right
	m_p_currentWorld->addEntityToMap(new Entity({ -1232, -1280, 32,  3264})); // Border top
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 528 * 2, -1280 + 768 * 2, 55, 64 })); //First sign on the left from spawn
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 835 * 2, -1280 + 1088 * 2, 55, 64 })); //First sign on the bottom from spawn
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 336 * 2, -1280 + 608 * 2, 192, 64 })); //Holy statue
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1344 * 2, -1280 + 624 * 2, 64, 64 })); //Shrine right from spawn

	MenuManager::getInstance().setGameHandler(this);
	MenuManager::getInstance().setCurrentWorld(m_p_currentWorld);

	return gameLoop();
}

TTF_Font* GameHandler::getFont(int fontSize)
{
	switch (fontSize) {
		case 45:
			return m_gameFonts[0];
			break;
		case 30:
			return m_gameFonts[1];
			break;
		case 34:
			return m_gameFonts[2];
			break;
	}
	return nullptr;
}

void GameHandler::checkCurrentWave()
{
	if (m_p_currentWorld->getEnemyVector()->size() > 0) {	//The current wave is still ongoing
		if (m_p_waveClock->checkClockState() && m_waveTimer > 0) {
			m_waveTimer--;
		}
		return;
	}

	if (m_waveTimer > 0) {
		if (m_waveTimer > m_deltaTime / 10.0f) {
			m_waveTimer -= m_deltaTime / 10.0f;
			m_p_currentWorld->getPlayer()->updateCoinCounter(m_deltaTime / 10.0f);
		}
		else {
			m_p_currentWorld->getPlayer()->updateCoinCounter(m_waveTimer);
			m_waveTimer = 0;
		}
		
	}	

	if (!m_p_currentWorld->getMerchantIsActive()) {
		m_waveCounter++;
		short enemiesToSpawn = 1 + m_waveCounter * 0;
		m_waveTimer = enemiesToSpawn * 10;		//10 seconds to defeat each enemy
		while (enemiesToSpawn > 0) {
			if (trySpawningEnemy())
				enemiesToSpawn--;
		}
	}

}

bool GameHandler::trySpawningEnemy()
{
	short enemyType = m_p_currentWorld->getRandomNumber(0, 1);		//Calculating what kind of enemy will spawn
	SDL_FPoint randomPosition = m_p_currentWorld->getRandomCoordinate(); //Random positon for the new enemy
	
	SDL_FRect tmpRectBounds{ randomPosition.x, randomPosition.y, 64, 64 };
	SDL_FRect tmpRectSprite = tmpRectBounds;
	short lives = 0;

	switch (enemyType) {
		case 0:
			lives = 1;
			break;
		case 1:
			tmpRectBounds.h = 32;
			tmpRectBounds.y += 32;
			lives = 1;
			break;
	}

	SDL_FRect windowRect = { 0, 0, 800, 640 };
	if (SDL_HasIntersectionF(&windowRect, &tmpRectBounds))
		return false;

	for (auto const& cursor : *m_p_currentWorld->getEnemyVector()) {
		if (SDL_HasIntersectionF(&tmpRectBounds, cursor->getBounds())) {
			return false;
		}
	}

	for (auto const& cursor : *m_p_currentWorld->getEntityVector()) {
		if (SDL_HasIntersectionF(&tmpRectBounds, cursor->getBounds())) {
			return false;
		}
	}

	m_p_currentWorld->addEnemyToMap(new Enemy(m_enemyTexturesIdle[enemyType], m_enemyTexturesWalk[enemyType], m_enemyTexturesHit[enemyType], tmpRectBounds, tmpRectSprite, lives));
	return true;
}

void GameHandler::renderWorldBackground()
{
	SDL_RenderCopy(m_p_renderer, m_miscTextures[0], NULL, NULL);
}

void GameHandler::renderHud()
{
	const SDL_FRect hudRect = { 10 + 30, 10 + 10, 320, 50 };
	SDL_FRect healthRect = { 35 + 30, 15 + 10, 290, 20 };
	SDL_FRect staminaRect = { 20 + 30, 40 + 10, 280, 10 };

	SDL_RenderCopy(m_p_renderer, m_hudTextures[5], NULL, NULL);	//Wood sign top left

	Player* p_player = m_p_currentWorld->getPlayer();
	
	float healthPercent = float(p_player->getCurrentLives()) / float(p_player->getMaxLives());
	healthRect.w *= healthPercent;
	SDL_Rect tmpTextureCoords = { 0, 0, 58 * healthPercent, 4 };
	SDL_RenderCopyF(m_p_renderer, m_hudTextures[1], &tmpTextureCoords, &healthRect); //Health

	float staminaPercent = p_player->getAttackCooldownPercent();
	if (staminaPercent == 1) {
		SDL_RenderCopyF(m_p_renderer, m_hudTextures[3], NULL, &hudRect); //Check
	}
	staminaRect.w = round(staminaRect.w * staminaPercent);
	SDL_RenderCopyF(m_p_renderer, m_hudTextures[2], NULL, &staminaRect); //Stamina
	SDL_RenderCopyF(m_p_renderer, m_hudTextures[0], NULL, &hudRect); //Border

	//----------------------------------------------------------------- Render wave counter
	SDL_RenderCopyF(m_p_renderer, m_hudTextures[4], NULL, NULL);
	const SDL_Color colorWaCo = { 229, 229, 203 }; //Color for the wave counter

	std::string displayText = "Wave " + std::to_string(m_waveCounter);
	SDL_Surface* surfaceText = TTF_RenderText_Solid(getFont(45), displayText.c_str(), colorWaCo);
	SDL_Texture* textureText = SDL_CreateTextureFromSurface(m_p_renderer, surfaceText);

	SDL_Rect textRect;
	textRect.x = 659 - surfaceText->w / 2;
	textRect.y = 20;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;

	SDL_RenderCopy(m_p_renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);

	//----------------------------------------------------------------- Render bottom hud
	
	SDL_Rect enemyBarRect{ 234, 576, 333, 30 };
	SDL_RenderCopy(m_p_renderer, m_hudTextures[6], NULL, NULL);
	int numberOfEnemies = int(m_p_currentWorld->getEnemyVector()->size());
	float enemyPercent = float(numberOfEnemies) / (m_waveCounter * 5.0f + 5.0f);
	enemyBarRect.w *= enemyPercent;
	SDL_RenderCopy(m_p_renderer, m_hudTextures[7], NULL, &enemyBarRect); // Enemy bar
	enemyBarRect.w = 333;
	SDL_RenderCopy(m_p_renderer, m_hudTextures[8], NULL, &enemyBarRect); // Border of enemy bar

	//----------------------------------------------------------------- Render enemy counter
	const SDL_Color colorEnCo = { 255, 181, 100 }; //Color for the enemy counter

	displayText = std::to_string(numberOfEnemies) + ((numberOfEnemies == 1) ? " enemy" : " enemies") + " left";
	surfaceText = TTF_RenderText_Solid(getFont(30), displayText.c_str(), colorEnCo);
	textureText = SDL_CreateTextureFromSurface(m_p_renderer, surfaceText);

	textRect.x = 400 - surfaceText->w / 2;
	textRect.y = 542;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;

	SDL_RenderCopy(m_p_renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);

	//----------------------------------------------------------------- Render wave timer
	const SDL_Color colorWaTi = { 240, 66, 66 }; //Color for the wave timer

	displayText = std::to_string(m_waveTimer);
	surfaceText = TTF_RenderText_Solid(getFont(34), displayText.c_str(), colorWaTi);
	textureText = SDL_CreateTextureFromSurface(m_p_renderer, surfaceText);

	textRect.x = 177 - surfaceText->w / 2;
	textRect.y = 558;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;

	SDL_RenderCopy(m_p_renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);

	//----------------------------------------------------------------- Render coin counter
	const SDL_Color colorCoiCo = { 255, 195, 50 }; //Color for the wave timer

	displayText = std::to_string(m_p_currentWorld->getPlayer()->getCoinCounter());
	surfaceText = TTF_RenderText_Solid(getFont(34), displayText.c_str(), colorCoiCo);
	textureText = SDL_CreateTextureFromSurface(m_p_renderer, surfaceText);

	textRect.x = 623 - surfaceText->w / 2;
	textRect.y = 558;
	textRect.w = surfaceText->w;
	textRect.h = surfaceText->h;

	SDL_RenderCopy(m_p_renderer, textureText, NULL, &textRect);
	SDL_FreeSurface(surfaceText);
	SDL_DestroyTexture(textureText);
}

void GameHandler::renderEverything(bool leftMouseButtonPressed)
{
	SDL_RenderClear(m_p_renderer);
	renderWorldBackground();
	m_p_currentWorld->renderWorld(m_p_renderer);
	renderHud();
	MenuManager::getInstance().interactWithMenu(leftMouseButtonPressed, m_p_renderer);		//This function also renders the menu
	SDL_RenderPresent(m_p_renderer);
}
