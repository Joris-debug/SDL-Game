#include "GameHandler.h"
#include "Interface.h"
#include "World.h"
#include "Resources.h"
#include <iostream>
#include <SDL_image.h>
int GameHandler::gameLoop()
{
	int x_input = 0, y_input = 0;
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
				case SDL_KEYDOWN:
					switch (m_p_interface_->getInputQueue()->key.keysym.sym)
					{
					case SDLK_s:
						y_input = 1;
						break;
					case SDLK_a:
						x_input = -1;
						break;
					case SDLK_d:
						x_input = 1;
						break;

					case SDLK_0:
						SDL_SetWindowFullscreen(m_p_interface_->getWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);

						break;
					case SDLK_1:
						SDL_SetWindowFullscreen(m_p_interface_->getWindow(), 0);
						SDL_SetWindowSize(m_p_interface_->getWindow(), STARTUP_SCREEN_WIDTH, STARTUP_SCREEN_HEIGHT);
						break;
					}
					break;

				case SDL_KEYUP:

					switch (m_p_interface_->getInputQueue()->key.keysym.sym)
					{

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

		m_p_interface_->getPixelPerPixel();
		m_p_currentWorld_->moveWorld(x_input, y_input, m_deltaTime_, m_p_interface_);
		renderEverything();

		}
		
	}

	m_p_interface_->waitForInput(300);

}

GameHandler::GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_, double *m_p_pixel_per_pixel)
{
	this->m_p_interface_ = m_p_interface_;
	this->m_p_renderer_ = m_p_renderer_;
	this->m_p_pixel_per_pixel_ = m_p_pixel_per_pixel;
}

GameHandler::~GameHandler()
{

}

int GameHandler::initLevel1()
{
	m_p_currentWorld_ = std::unique_ptr<World>(new World(IMG_Load(RSC_LEVEL_1), { -500,-500,1792 * 2,768 * 2 }, m_p_renderer_));
	m_p_currentWorld_->addVinicityToMap(new Vicinity(IMG_Load(RSC_CASTLE_BACKGROUND_LAYER_01), { 0,-100,426 * 2,384 * 2 }, m_p_renderer_));
	m_p_currentWorld_->addVinicityToMap(new Vicinity(IMG_Load(RSC_CASTLE_BACKGROUND_LAYER_02), { 0,-100,426 * 2,384 * 2 }, m_p_renderer_));
	m_p_currentWorld_->addVinicityToMap(new Vicinity(IMG_Load(RSC_CASTLE_BACKGROUND_LAYER_03B), { 0,-100,426 * 2,384 * 2 }, m_p_renderer_));
	/*m_p_currentWorld_->addVinicityToMap(new Vicinity(IMG_Load(RSC_CASTLE_BACKGROUND_LAYER_04), { 0,-300,426 * 2,384 * 2 }, m_p_renderer_));*/
	
	return gameLoop();
}

void GameHandler::renderEverything()
{
	SDL_RenderClear(m_p_renderer_);
	m_p_currentWorld_->renderWorld(m_p_renderer_, *m_p_pixel_per_pixel_, m_p_interface_);
	SDL_RenderPresent(m_p_renderer_);
}
