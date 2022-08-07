#include "World.h"
#include "Interface.h"
#include "Resources.h"
#include "SDL_image.h"
#include "Player.h"
World::World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer) : Vicinity(surface, m_bounds_, renderer)
{
	m_p_player_ = std::unique_ptr<Player>(new Player(renderer));
}

World::~World()
{
}

World::World()
{
}

void World::moveWorld(int x, int y, double deltaTime, Interface* p_Interface)
{
	if (x == 0 && y == 0)
		return;

	SDL_FRect *temp = this->getBounds();
	temp->x += x * deltaTime;
	temp->y += y * deltaTime;

	deltaTime /= 2;

	double layerCounter = m_listOfLayers_.size();
	

	int screenWidth;
	int screenHeight;
	SDL_GetWindowSize(p_Interface->getWindow(), &screenWidth, &screenHeight);

	for (Vicinity* cursor : m_listOfLayers_)
	{
		cursor->moveVicinity(x * deltaTime / layerCounter, y * deltaTime / layerCounter, screenHeight);
		layerCounter--;
	}
}

void World::renderWorld(SDL_Renderer* renderer, double pixel_per_pixel, Interface* p_Interface)
{
	SDL_RenderClear(renderer);
	int screenWidth;
	int screenHeight;

	SDL_GetWindowSize(p_Interface->getWindow(), &screenWidth, &screenHeight);

	for (Vicinity* cursor : m_listOfLayers_)
	{
		cursor->renderVicinity(renderer, pixel_per_pixel, screenWidth);	//Funktion to render parralax background
	}

	//Render the actual level
	SDL_FRect tmp = *this->getBounds();	
	tmp.x = round(tmp.x * pixel_per_pixel);
	tmp.y = round(tmp.y * pixel_per_pixel);
	tmp.w = round(tmp.w * pixel_per_pixel);
	tmp.h = round(tmp.h * pixel_per_pixel);
	SDL_RenderCopyF(renderer, this->getTexture(), NULL, &tmp);

	m_p_player_->animatePlayer();
	m_p_player_->renderPlayer(renderer, pixel_per_pixel);

}
