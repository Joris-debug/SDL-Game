#include "GameHandler.h"
#include "Interface.h"
#include "World.h"
#include "Resources.h"
#include "Beetle.h"
#include "Player.h"
#include "SDL_image.h"
#include "Effect.h"
#include "MenuManager.h"
#include "SoundHandler.h"
#include <string>
int GameHandler::gameLoop()
{
	int x_input = 0, y_input = 0;
	bool leftMouseButtonPressed = false, keyPressed = false, eKeyPressed = false, fKeyPressed = false, escKeyPressed = false;
	Uint32 currentTime = SDL_GetTicks(); //Calculate delta time
	Uint32 lastTime = currentTime;
	while (true)
	{
		currentTime = SDL_GetTicks();
		m_deltaTime = float(currentTime - lastTime);

		if (m_deltaTime >= 1000.0f / 60.0f) //Limit FPS auf 60
		{
			if (m_deltaTime > 150.0f)	//Capping deltatime
				m_deltaTime = 150.0f;

			lastTime = currentTime;
			while (SDL_PollEvent(Interface::getInstance().getInputQueue()) != 0)
			{

				switch (Interface::getInstance().getInputQueue()->type)
				{
				case SDL_QUIT:
					exit(1);
					break;

				case SDL_MOUSEBUTTONDOWN:
					leftMouseButtonPressed = true;
					break;

				case SDL_KEYDOWN:
					keyPressed = true;
					switch (Interface::getInstance().getInputQueue()->key.keysym.sym)
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

					case SDLK_ESCAPE:
						escKeyPressed = true;
						break;
					}
					break;

				case SDL_KEYUP:

					switch (Interface::getInstance().getInputQueue()->key.keysym.sym)
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

			switch (m_gameState) {
				case GameStates::hasEnded:
					goto exit_loop;	//Cant break out of loop inside a switch

				case GameStates::isRunning:
					checkCurrentWave();
					if (!m_p_menuManager->checkIfMenuOpen()) {		//No window is open

						if (leftMouseButtonPressed) {
							m_p_currentWorld->triggerPlayerAttack();
						}
						if (eKeyPressed && m_p_currentWorld->talkToMerchant()) {
							m_p_menuManager->openMenu(Menus::shop);
						}
						if (escKeyPressed) {
							m_p_menuManager->openMenu(Menus::pause);
						}
					}
					else {	//Player doesnt move when a window is open
						x_input = 0;
						y_input = 0;
						if (escKeyPressed) {
							 m_p_menuManager->tryClosingMenu();							
						}
					}



					if (!m_p_currentWorld->getPlayer()->getCurrentLives()) {		//Player is GameOver
						x_input = 0;
						y_input = 0;
						m_p_menuManager->openMenu(Menus::gameOver);
					}

					m_p_currentWorld->moveWorld(x_input, y_input, 0.2f * m_deltaTime);
					break;

				case GameStates::isStarting:
					if (keyPressed && m_p_newMenuOpened->checkClockState()) {
						m_p_menuManager->closeMenu();
						SoundHandler::getInstance().playClickSound();
					}
					break;
			}

			Interface::getInstance().calculatePixelPerPixel();
			Interface::getInstance().displayFPS(m_deltaTime);

			renderEverything(leftMouseButtonPressed);
			leftMouseButtonPressed = false;
			eKeyPressed = false;
			keyPressed = false;
			escKeyPressed = false;
		}

	}

	exit_loop:;
	return 0;
}

GameHandler::GameHandler(SDL_Renderer* m_p_renderer_)
{
	this->m_p_renderer = m_p_renderer_;
	this->m_deltaTime = 1;
	m_waveCounter = 0;
	m_waveTimer = 0;
	m_p_waveClock = new Clock(1000);
	m_p_newMenuOpened = new Clock(1000);
	m_p_currentWorld = nullptr;
	m_p_menuManager = nullptr;
	m_gameState = GameStates::isStarting;

	m_p_randomNumberEngine = new std::mt19937(Uint32(this));
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

	//Load all Beetle Spritesheets
	p_tmpSurface = IMG_Load(RSC_BEETLE_IDLE);
	m_enemyTexturesIdle.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_BEETLE_WALK);
	m_enemyTexturesWalk.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_BEETLE_HIT);
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
	m_gameFonts.push_back(TTF_OpenFont(RSC_8BIT_FONT, 75));	//Font used for tombstone
	m_gameFonts.push_back(TTF_OpenFont(RSC_PIXELSPLITTER_FONT, 40));	//Font used for pause menu volume buttons
}

GameHandler::~GameHandler()
{
	delete m_p_newMenuOpened;
	delete m_p_waveClock;
	delete m_p_currentWorld;
	delete m_p_menuManager;

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
	m_p_currentWorld = new World(IMG_Load(RSC_LEVEL_1), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer, m_p_randomNumberEngine, new Effect({0, 0, 256, 160}, m_effectTextures[0]));
	m_p_currentWorld->addVinicityToMap(new Vicinity(IMG_Load(RSC_LEVEL_1_TOP), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer));

	m_p_currentWorld->addEntityToMap(new Entity({ -1232, -1280, 3264, 32 })); // Border left
	m_p_currentWorld->addEntityToMap(new Entity({ -1232, -1280 + 1616*2, 3264, 32 })); // Border bottom
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1616 * 2, -1280, 32, 3264 })); // Border right
	m_p_currentWorld->addEntityToMap(new Entity({ -1232, -1280, 32,  3264})); // Border top
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 528 * 2, -1280 + 768 * 2, 55, 64 })); //First sign on the left from spawn
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 835 * 2, -1280 + 1088 * 2, 55, 64 })); //First sign on the bottom from spawn
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 336 * 2, -1280 + 608 * 2, 192, 64 })); //Holy statue
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1344 * 2, -1280 + 624 * 2, 64, 64 })); //Shrine right from spawn
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 288 * 2, -1280 + 224 * 2, 32, 64 })); //First tree top left
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 400 * 2, -1280 + 144 * 2, 128, 64 })); //Chest an vase
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 640 * 2, -1280 + 112 * 2, 32, 54 })); //Second tree top left
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 736 * 2, -1280 + 112 * 2, 54, 96 })); //Left bench
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 788 * 2, -1280 + 80 * 2, 112, 62 })); //Top bench
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 869 * 2, -1280 + 112 * 2, 54, 96 })); //Right bench
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1568 * 2, -1280 + 112 * 2, 32, 56 })); //Last tree top right
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 176 * 2, -1280 + 400 * 2, 32, 60 })); //First tree second row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 512 * 2, -1280 + 320 * 2, 32, 62 })); //Second tree second row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1040 * 2, -1280 + 256 * 2, 32, 60 })); //Third tree second row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1216 * 2, -1280 + 288 * 2, 32, 62 })); //Fourth tree second row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1392 * 2, -1280 + 288 * 2, 32, 62 })); //Fifth tree second row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 336 * 2, -1280 + 528 * 2, 32, 60 })); //First tree third row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 592 * 2, -1280 + 464 * 2, 32, 54 })); //Second tree third row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1104 * 2, -1280 + 448 * 2, 32, 62 })); //Third tree third row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1360 * 2, -1280 + 400 * 2, 64, 64 })); //First crate left
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1440 * 2, -1280 + 352 * 2, 64, 64 })); //Top crate
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1440 * 2, -1280 + 416 * 2, 64, 64 })); //Left barrel
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1504 * 2, -1280 + 368 * 2, 64, 64 })); //Right barrel
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1504 * 2, -1280 + 448 * 2, 64, 64 })); //Right crate
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1392 * 2, -1280 + 463 * 2, 64, 38 })); //Stone pile top left
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 144 * 2, -1280 + 592 * 2, 32, 54 })); //First tree fourth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 720 * 2, -1280 + 592 * 2, 32, 60 })); //Second tree fourth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1216 * 2, -1280 + 640 * 2, 32, 62 })); //Third tree fourth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1472 * 2, -1280 + 736 * 2, 32, 60 })); //Fourth tree fourth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 752 * 2, -1280 + 1378 * 2, 128, 94 })); //Brocken well at the bottom
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 240 * 2, -1280 + 976 * 2, 32, 60 })); //First tree fifth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 576 * 2, -1280 + 960 * 2, 32, 52 })); //Second tree fifth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1056 * 2, -1280 + 976 * 2, 32, 56 })); //Third tree fifth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1344 * 2, -1280 + 960 * 2, 32, 54 })); //Third tree fifth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1488 * 2, -1280 + 1024 * 2, 32, 60 })); //Fifth tree fifth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 96 * 2, -1280 + 1120 * 2, 32, 56 })); //First tree sixth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 368 * 2, -1280 + 1104 * 2, 32, 60 })); //Second tree sixth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 720 * 2, -1280 + 1072 * 2, 32, 58 })); //Third tree sixth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1155 * 2, -1280 + 1072 * 2, 52, 60 })); //Entrance stone graveyard
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1216 * 2, -1280 + 1056 * 2, 64, 64 })); //Top row first grave
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1280 * 2, -1280 + 1056 * 2, 64, 64 })); //Top row second grave
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1344 * 2, -1280 + 1056 * 2, 64, 64 })); //Top row third grave
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1408 * 2, -1280 + 1056 * 2, 64, 64 })); //Top row fourth grave
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1216 * 2, -1280 + 1152 * 2, 64, 64 })); //Middle row first grave
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1280 * 2, -1280 + 1152 * 2, 64, 64 })); //Middle row second grave
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1344 * 2, -1280 + 1152 * 2, 64, 64 })); //Middle row third grave
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1408 * 2, -1280 + 1152 * 2, 64, 64 })); //Middle row fourth grave
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1216 * 2, -1280 + 1248 * 2, 64, 96 })); //Left coffin
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1296 * 2, -1280 + 1258 * 2, 128, 64 })); //Middle coffin
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1408 * 2, -1280 + 1248 * 2, 64, 96 })); //Right coffin
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 256 * 2, -1280 + 1184 * 2, 64, 64 })); //Pillar top left
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 384 * 2, -1280 + 1184 * 2, 64, 64 })); //Pillar top right
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 256 * 2, -1280 + 1312 * 2, 64, 64 })); //Pillar bottom left
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 384 * 2, -1280 + 1312 * 2, 64, 64 })); //Pillar bottom right
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 512 * 2, -1280 + 1184 * 2, 32, 60 })); //First tree seventh row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 512 * 2, -1280 + 1184 * 2, 32, 60 })); //First tree seventh row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1472 * 2, -1280 + 1232 * 2, 32, 60 })); //Second tree seventh row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 609 * 2, -1280 + 1376 * 2, 32, 62 })); //First tree eigth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1040 * 2, -1280 + 1360 * 2, 32, 60 })); //Second tree eigth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1296 * 2, -1280 + 1408 * 2, 32, 60 })); //Third tree eigth row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 160 * 2, -1280 + 1440 * 2, 32, 62 })); //First tree last row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 496 * 2, -1280 + 1472 * 2, 32, 56 })); //Second tree last row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 960 * 2, -1280 + 1488 * 2, 32, 56 })); //Third tree last row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1168 * 2, -1280 + 1504 * 2, 32, 58 })); //Fourth tree last row
	m_p_currentWorld->addEntityToMap(new Entity({ -1232 + 1488 * 2, -1280 + 1520 * 2, 32, 60 })); //Fourth tree last row

	m_p_menuManager = new MenuManager(m_p_renderer, this, m_p_currentWorld);



	return gameLoop();
}

void GameHandler::resetWorld()
{
	delete m_p_currentWorld;
	delete m_p_menuManager;
	m_waveCounter = 0;
	m_waveTimer = 0;
	m_p_newMenuOpened->setStartPoint(SDL_GetTicks());
	m_gameState = GameStates::isStarting;
}

TTF_Font* GameHandler::getFont(Fonts font, int fontSize)
{
	switch (font) {
	case Fonts::eightBit:
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
		case 75:
			return m_gameFonts[3];
			break;
		}
		break;

	case Fonts::pixelSplitter:
		switch (fontSize) {
		case 40:
			return m_gameFonts[4];
			break;
		}
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
		if (m_waveTimer > int(m_deltaTime / 10.0f)) {
			m_waveTimer -= int(m_deltaTime / 10.0f);
			m_p_currentWorld->getPlayer()->updateCoinCounter(int(m_deltaTime / 10.0f));
		}
		else {
			m_p_currentWorld->getPlayer()->updateCoinCounter(m_waveTimer);
			m_waveTimer = 0;
		}
		
	}	

	if (!m_p_currentWorld->getMerchantIsActive()) {
		m_waveCounter++;
		short enemiesToSpawn = 5 + m_waveCounter * 5;
		m_waveTimer = enemiesToSpawn * 10;		//10 seconds to defeat each enemy
		while (enemiesToSpawn > 0) {
			if (trySpawningEnemy())
				enemiesToSpawn--;
		}
	}

}

bool GameHandler::trySpawningEnemy()
{
	short enemyType = m_p_currentWorld->getRandomNumber(0, 2);		//Calculating what kind of enemy will spawn
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
		case 2:
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

	Enemy* p_enemy;

	if (enemyType == 2) {
		p_enemy = new Beetle(m_enemyTexturesIdle[enemyType], m_enemyTexturesWalk[enemyType], m_enemyTexturesHit[enemyType], tmpRectBounds, tmpRectSprite, lives);
	}
	else {
		p_enemy = new Enemy(m_enemyTexturesIdle[enemyType], m_enemyTexturesWalk[enemyType], m_enemyTexturesHit[enemyType], tmpRectBounds, tmpRectSprite, lives);
	}

	m_p_currentWorld->addEnemyToMap(p_enemy);
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
	SDL_Surface* surfaceText = TTF_RenderText_Solid(getFont(Fonts::eightBit, 45), displayText.c_str(), colorWaCo);
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
	surfaceText = TTF_RenderText_Solid(getFont(Fonts::eightBit, 30), displayText.c_str(), colorEnCo);
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
	surfaceText = TTF_RenderText_Solid(getFont(Fonts::eightBit, 34), displayText.c_str(), colorWaTi);
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
	surfaceText = TTF_RenderText_Solid(getFont(Fonts::eightBit, 34), displayText.c_str(), colorCoiCo);
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
	
	if (m_gameState == GameStates::isRunning)
		renderHud();

	m_gameState = m_p_menuManager->interactWithMenu(leftMouseButtonPressed, m_p_renderer, m_deltaTime);	//This function also renders the menu

	SDL_RenderPresent(m_p_renderer);
}
