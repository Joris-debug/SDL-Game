#include "World.h"

World::World(SDL_Surface* surface, SDL_Rect m_bounds_, SDL_Renderer* renderer) : Vicinity(surface, m_bounds_, renderer)
{

}

void World::moveWorld(int x, int y)
{

}

void World::renderWorld(SDL_Renderer* renderer, double pixel_per_pixel)
{
	SDL_RenderClear(renderer);
	for (Vicinity* cursor : m_listOfLayers_)
	{
		cursor->renderVicinity(renderer, pixel_per_pixel);
	}
	this->renderVicinity(renderer, pixel_per_pixel);
}
