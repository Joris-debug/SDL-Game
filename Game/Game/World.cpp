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
	m_p_player_->animatePlayer(x, y);

	if (x == 0 && y == 0) {
		return;
	}
		

	SDL_FRect *temp = this->getBounds();
	temp->x += x * deltaTime;
	temp->y += y * deltaTime;

		

	int screenWidth;
	int screenHeight;
	SDL_GetWindowSize(p_Interface->getWindow(), &screenWidth, &screenHeight);


	//m_p_topMap_->moveVicinity(x * deltaTime, y * deltaTime , screenHeight);
}

void World::renderWorld(SDL_Renderer* renderer, double pixel_per_pixel, Interface* p_Interface)
{
	SDL_RenderClear(renderer);
	int screenWidth;
	int screenHeight;

	SDL_GetWindowSize(p_Interface->getWindow(), &screenWidth, &screenHeight);

	//m_p_topMap_->renderVicinity(renderer, pixel_per_pixel, screenWidth);	//Funktion to render top map
	
	//Render the actual level
	SDL_FRect playerCoords = m_p_player_->getBounds();
	SDL_FRect tmp = m_bounds_;	
	tmp.x = round(tmp.x * pixel_per_pixel);
	tmp.y = round((tmp.y * pixel_per_pixel));
	tmp.w = round(tmp.w * pixel_per_pixel);
	tmp.h = round(tmp.h * pixel_per_pixel);
	std::cout << tmp.h/pixel_per_pixel << std::endl;
	SDL_RenderCopyF(renderer, m_p_texture_, NULL, &tmp);

	m_p_player_->renderPlayer(renderer, pixel_per_pixel);

}
