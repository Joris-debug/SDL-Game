#include "GameHandler.h"
#include "Interface.h"
#include "World.h"
#include "Resources.h"
#include "Enemy.h"
#include "Player.h"
#include "SDL_image.h"
#include <string>
int GameHandler::gameLoop()
{
	int x_input = 0, y_input = 0, attackTrigger = false;
	Uint32 currentTime = SDL_GetTicks(); //Calculate delta time
	Uint32 lastTime = currentTime;
	while (true)
	{
		currentTime = SDL_GetTicks();
		m_deltaTime_ = (currentTime - lastTime);

		if (m_deltaTime_ > float(1000 / 60)) //Limit FPS auf 60
		{
			lastTime = currentTime;
			while (SDL_PollEvent(m_p_interface_->getInputQueue()) != 0)
			{

				switch (m_p_interface_->getInputQueue()->type)
				{
				case SDL_QUIT:
					exit(1);
					break;

				case SDL_KEYDOWN:
					switch (m_p_interface_->getInputQueue()->key.keysym.sym)
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

						//case SDLK_0:
						//	SDL_SetWindowFullscreen(m_p_interface_->getWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
						//	break;

						//case SDLK_1:
						//	SDL_SetWindowFullscreen(m_p_interface_->getWindow(), 0);
						//	SDL_SetWindowSize(m_p_interface_->getWindow(), STARTUP_SCREEN_WIDTH, STARTUP_SCREEN_HEIGHT);
						//	break;
					}
					break;

				case SDL_KEYUP:

					switch (m_p_interface_->getInputQueue()->key.keysym.sym)
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


				case SDL_MOUSEBUTTONDOWN:
					attackTrigger = true;
					break;

				}
			}
			checkCurrentWave();

			if (attackTrigger) {
				m_p_currentWorld_->triggerPlayerAttack();
				attackTrigger = false;
			}

			m_p_interface_->getPixelPerPixel();
			m_p_currentWorld_->moveWorld(x_input, y_input, m_deltaTime_ * 0.2);
			renderEverything();

			if (!m_p_currentWorld_->getPlayer()->get()->getCurrentLives()) {
				break;
			}
		}

	}

	m_p_interface_->waitForInput(1000);
	return 0;
}

GameHandler::GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_)
{
	this->m_p_interface_ = m_p_interface_;
	this->m_p_renderer_ = m_p_renderer_;
	this->m_deltaTime_ = 1;
	m_waveCounter_ = 0;

	SDL_Surface* p_tmpSurface;

	//Load all Mantis Spritesheets
	p_tmpSurface = IMG_Load(RSC_MANTIS_IDLE);
	m_enemyTexturesIdle_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MANTIS_WALK);
	m_enemyTexturesWalk_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));	
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MANTIS_HIT);
	m_enemyTexturesHit_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//Load all Maggot Spritesheets
	p_tmpSurface = IMG_Load(RSC_MAGGOT_IDLE);
	m_enemyTexturesIdle_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MAGGOT_WALK);
	m_enemyTexturesWalk_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MAGGOT_HIT);
	m_enemyTexturesHit_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//Load all hud textures
	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_BORDER);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_LIVES);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_STAMINA);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_CHECK);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_WOODEN_BOARD);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_BOTTOM_HUD);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_ENEMYBAR);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_ENEMYBAR_BORDER);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//Load misc textures
	p_tmpSurface = IMG_Load(RSC_WORLD_BACKGROUND);
	m_miscTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//Load all fonts
	m_gameFonts_.push_back(TTF_OpenFont(RSC_8BIT_FONT, 45));	//Font used for the wave counter
}

GameHandler::~GameHandler()
{
	int numberOfElements = m_enemyTexturesIdle_.size();
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_enemyTexturesIdle_.back());
		m_enemyTexturesIdle_.pop_back();
		SDL_DestroyTexture(m_enemyTexturesWalk_.back());
		m_enemyTexturesWalk_.pop_back();
		SDL_DestroyTexture(m_enemyTexturesHit_.back());
		m_enemyTexturesHit_.pop_back();
	}

	numberOfElements = m_hudTextures_.size();
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_hudTextures_.back());
		m_hudTextures_.pop_back();
	}

	numberOfElements = m_miscTextures_.size();
	for (int i = 0; i < numberOfElements; i++) {
		SDL_DestroyTexture(m_miscTextures_.back());
		m_miscTextures_.pop_back();
	}

	//Delete all fonts
	numberOfElements = m_gameFonts_.size();
	for (int i = 0; i < numberOfElements; i++) {
		TTF_CloseFont(m_gameFonts_.back());
		m_gameFonts_.pop_back();
	}
}

int GameHandler::initWorld()
{
	m_p_currentWorld_ = std::unique_ptr<World>(new World(IMG_Load(RSC_LEVEL_1), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer_, &m_randomNumberEngine_));
	m_p_currentWorld_->addVinicityToMap(new Vicinity(IMG_Load(RSC_LEVEL_1_TOP), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer_));

	m_p_currentWorld_->addEntityToMap(new Entity({ -1232, -1280, 3264, 32 })); // Border left
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232, -1280 + 1616*2, 3264, 32 })); // Border bottom
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 1616 * 2, -1280, 32, 3264 })); // Border right
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232, -1280, 32,  3264})); // Border top
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 528 * 2, -1280 + 768 * 2, 55, 64 })); //First sign on the left from spawn
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 835 * 2, -1280 + 1088 * 2, 55, 64 })); //First sign on the bottom from spawn
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 336 * 2, -1280 + 608 * 2, 192, 64 })); //Holy statue
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 1344 * 2, -1280 + 624 * 2, 64, 64 })); //Shrine right from spawn

	//m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[0], m_enemyTexturesWalk_[0], m_enemyTexturesHit_[0], { 800, 200, 64, 64 }, { 800, 200, 64, 64 }, 2));
	//m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[1], m_enemyTexturesWalk_[1], m_enemyTexturesHit_[1], { -800, -68, 64, 32 }, { -800, -100, 64, 64 }, 1));


	return gameLoop();
}

void GameHandler::checkCurrentWave()
{
	if (m_p_currentWorld_->getEnemyVector()->size() > 0) {	//The current wave is still ongoing
		return;
	}

	m_waveCounter_++;
	short enemiesToSpawn = 5 + m_waveCounter_ * 5;

	while (enemiesToSpawn > 0) {
		if (trySpawningEnemy())
			enemiesToSpawn--;
	}

}

bool GameHandler::trySpawningEnemy()
{
	short enemyType = m_p_currentWorld_->getRandomNumber(0, 1);		//Calculating what kind of enemy will spawn
	SDL_FPoint randomPosition = m_p_currentWorld_->getRandomCoordinate(); //Random positon for the new enemy
	
	SDL_FRect tmpRectBounds{ randomPosition.x, randomPosition.y, 64, 64 };
	SDL_FRect tmpRectSprite = tmpRectBounds;
	short lives;

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

	SDL_FRect windowRect = { 0, 0, 640, 800 };
	if (SDL_HasIntersectionF(&windowRect, &tmpRectBounds))
		return false;

	for (auto const& cursor : *m_p_currentWorld_->getEnemyVector()) {
		if (SDL_HasIntersectionF(&tmpRectBounds, cursor->getBounds())) {
			return false;
		}
	}

	for (auto const& cursor : *m_p_currentWorld_->getEntityVector()) {
		if (SDL_HasIntersectionF(&tmpRectBounds, cursor->getBounds())) {
			return false;
		}
	}

	m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[enemyType], m_enemyTexturesWalk_[enemyType], m_enemyTexturesHit_[enemyType], tmpRectBounds, tmpRectSprite, lives));
	return true;
}

void GameHandler::renderWorldBackground()
{
	SDL_RenderCopy(m_p_renderer_, m_miscTextures_[0], NULL, NULL);
}

void GameHandler::renderHud()
{
	const SDL_FRect hudRect = { 10, 10, 320, 50 };
	SDL_FRect healthRect = { 35, 15, 290, 20 };
	SDL_FRect staminaRect = { 20, 40, 280, 10 };

	Player* p_player = m_p_currentWorld_->getPlayer()->get();
	
	float healthPercent = float(p_player->getCurrentLives()) / float(p_player->getMaxLives());
	healthRect.w *= healthPercent;
	SDL_Rect tmpTextureCoords = { 0, 0, 58 * healthPercent, 4 };
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[1], &tmpTextureCoords, &healthRect); //Health

	float staminaPercent = (SDL_GetTicks() - p_player->getLastAttack()) / PLAYER_ATTACK_COOLDOWN;
	if (staminaPercent > 1) {
		staminaPercent = 1;
		SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[3], NULL, &hudRect); //Check
	}
	staminaRect.w = round(staminaRect.w * staminaPercent);
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[2], NULL, &staminaRect); //Stamina
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[0], NULL, &hudRect); //Border

	//----------------------------------------------------------------- Render wave counter
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[4], NULL, NULL);
	const SDL_Color colorWaCo = { 229, 229, 203 }; //Color for the wave counter

	std::string displayText = "Wave " + std::to_string(m_waveCounter_);
	SDL_Surface* surfaceWaCo = TTF_RenderText_Solid(m_gameFonts_[0], displayText.c_str(), colorWaCo);
	SDL_Texture* textureWaCo = SDL_CreateTextureFromSurface(m_p_renderer_, surfaceWaCo);

	SDL_Rect WaCoRect;
	WaCoRect.x = 659 - surfaceWaCo->w / 2;
	WaCoRect.y = 44 - surfaceWaCo->h / 2;
	WaCoRect.w = surfaceWaCo->w;
	WaCoRect.h = surfaceWaCo->h;

	SDL_RenderCopy(m_p_renderer_, textureWaCo, NULL, &WaCoRect);
	SDL_FreeSurface(surfaceWaCo);
	SDL_DestroyTexture(textureWaCo);

	//----------------------------------------------------------------- Render bottom hud
	
	SDL_Rect enemyBarRect{ 234, 576, 333, 30 };
	SDL_RenderCopy(m_p_renderer_, m_hudTextures_[5], NULL, NULL);
	float enemyPercent = m_p_currentWorld_->getEnemyVector()->size() / (m_waveCounter_ * 5.0 + 5.0);
	enemyBarRect.w *= enemyPercent;
	SDL_RenderCopy(m_p_renderer_, m_hudTextures_[6], NULL, &enemyBarRect); // Enemy bar
	enemyBarRect.w = 333;
	SDL_RenderCopy(m_p_renderer_, m_hudTextures_[7], NULL, &enemyBarRect); // Border of enemy bar
}

void GameHandler::renderEverything()
{
	SDL_RenderClear(m_p_renderer_);
	renderWorldBackground();
	m_p_currentWorld_->renderWorld(m_p_renderer_);
	renderHud();
	SDL_RenderPresent(m_p_renderer_);
}
