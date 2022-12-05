#include "GameHandler.h"
#include "Interface.h"
#include "World.h"
#include "Resources.h"
#include "Enemy.h"
#include <SDL_image.h>
int GameHandler::gameLoop()
{
	int x_input = 0, y_input = 0, attackTrigger = false;
	Uint32 currentTime = SDL_GetTicks(); //Calculate delta time
	while (true)
	{
		Uint32 lastTime = currentTime;
		currentTime = SDL_GetTicks();
		m_deltaTime_ = (currentTime - lastTime) * 0.2;

		if (m_deltaTime_ < float(1000 / 60)) //Limit FPS auf 60
		{

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

		m_p_interface_->getPixelPerPixel();
		if (attackTrigger) {	
			m_p_currentWorld_->triggerPlayerAttack();
		}
		m_p_currentWorld_->moveWorld(x_input, y_input, m_deltaTime_, m_p_interface_);
		renderEverything();
		attackTrigger = false;
		}
		
	}

	m_p_interface_->waitForInput(300);

}

GameHandler::GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_, double *m_p_pixel_per_pixel)
{
	this->m_p_interface_ = m_p_interface_;
	this->m_p_renderer_ = m_p_renderer_;
	this->m_p_pixel_per_pixel_ = m_p_pixel_per_pixel;
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


}

GameHandler::~GameHandler()
{
	int numberOfTextures = m_enemyTexturesIdle_.size();

	for (int i = 0; i < numberOfTextures; i++) {
		SDL_DestroyTexture(m_enemyTexturesIdle_.front());
		m_enemyTexturesIdle_.pop_back();
		SDL_DestroyTexture(m_enemyTexturesWalk_.front());
		m_enemyTexturesWalk_.pop_back();
		SDL_DestroyTexture(m_enemyTexturesHit_.front());
		m_enemyTexturesHit_.pop_back();
	}
}

int GameHandler::initLevel1()
{
	m_p_currentWorld_ = std::unique_ptr<World>(new World(IMG_Load(RSC_LEVEL_1), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer_));
	m_p_currentWorld_->addVinicityToMap(new Vicinity(IMG_Load(RSC_LEVEL_1_TOP), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer_));
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 528 * 2, -1280 + 768 * 2, 55, 64 })); //First sign on the left from spawn
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 336 * 2, -1280 + 608 * 2, 192, 64 })); //Holy statue
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 1344 * 2, -1280 + 624 * 2, 64, 64 })); //Shrine right from spawn


	m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[0], m_enemyTexturesWalk_[0], m_enemyTexturesHit_[0], { 500, 100, 64, 64 }, { 500, 100, 64, 64 }, 2));
	m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[0], m_enemyTexturesWalk_[0], m_enemyTexturesHit_[0], { 100, 100, 64, 64 }, { 100, 100, 64, 64 }, 2));
	m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[1], m_enemyTexturesWalk_[1], m_enemyTexturesHit_[1], { -100, -68, 64, 32 }, { -100, -100, 64, 64 }, 1));


	return gameLoop();
}

void GameHandler::renderEverything()
{
	SDL_RenderClear(m_p_renderer_);
	m_p_currentWorld_->renderWorld(m_p_renderer_, *m_p_pixel_per_pixel_, m_p_interface_);
	SDL_RenderPresent(m_p_renderer_);
}
