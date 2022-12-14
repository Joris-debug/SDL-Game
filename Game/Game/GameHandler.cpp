#include "GameHandler.h"
#include "Interface.h"
#include "World.h"
#include "Resources.h"
#include "Enemy.h"
#include "Player.h"
#include <SDL_image.h>
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
			if (attackTrigger) {	
				m_p_currentWorld_->triggerPlayerAttack();
				attackTrigger = false;
			}

			m_p_interface_->getPixelPerPixel();
			m_p_currentWorld_->moveWorld(x_input, y_input, m_deltaTime_ * 0.2, m_p_interface_);
			renderEverything();
		}
		
	}

	m_p_interface_->waitForInput(300);

}

GameHandler::GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_)
{
	this->m_p_interface_ = m_p_interface_;
	this->m_p_renderer_ = m_p_renderer_;
	this->m_deltaTime_ = 1;

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
}

GameHandler::~GameHandler()
{
	int numberOfTextures = m_enemyTexturesIdle_.size();

	for (int i = 0; i < numberOfTextures; i++) {
		SDL_DestroyTexture(m_enemyTexturesIdle_.back());
		m_enemyTexturesIdle_.pop_back();
		SDL_DestroyTexture(m_enemyTexturesWalk_.back());
		m_enemyTexturesWalk_.pop_back();
		SDL_DestroyTexture(m_enemyTexturesHit_.back());
		m_enemyTexturesHit_.pop_back();
	}

	numberOfTextures = m_hudTextures_.size();

	for (int i = 0; i < numberOfTextures; i++) {
		SDL_DestroyTexture(m_hudTextures_.back());
		m_hudTextures_.pop_back();
	}
}

int GameHandler::initLevel1()
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

	m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[0], m_enemyTexturesWalk_[0], m_enemyTexturesHit_[0], { 500, 100, 64, 64 }, { 500, 100, 64, 64 }, 2));
	m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[0], m_enemyTexturesWalk_[0], m_enemyTexturesHit_[0], { 100, 100, 64, 64 }, { 100, 100, 64, 64 }, 2));
	m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[1], m_enemyTexturesWalk_[1], m_enemyTexturesHit_[1], { -100, -68, 64, 32 }, { -100, -100, 64, 64 }, 1));


	return gameLoop();
}

void GameHandler::renderHud()
{
	SDL_FRect hudRect = { 10, 10, 64 * 5, 10 * 5};
	SDL_FRect staminaRect = { 20, 10+6*5, 56 * 5, 2 * 5 };
	
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[1], NULL, &hudRect); //Health

	float staminaPercent = (SDL_GetTicks() - m_p_currentWorld_->getPlayer()->get()->getLastAttack()) / 5000.0;
	if (staminaPercent > 1) {
		staminaPercent = 1;
		SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[3], NULL, &hudRect); //Check
	}
	staminaRect.w = round(staminaRect.w * staminaPercent);
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[2], NULL, &staminaRect); //Stamina
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[0], NULL, &hudRect); //Border
}

void GameHandler::renderEverything()
{
	SDL_RenderClear(m_p_renderer_);
	m_p_currentWorld_->renderWorld(m_p_renderer_);
	renderHud();
	SDL_RenderPresent(m_p_renderer_);
}
