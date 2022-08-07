#include "Entity.h"

Entity::Entity(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer)
{
	this->m_p_texture_ = SDL_CreateTextureFromSurface(renderer, surface);
	this->m_bounds_ = m_bounds_;
	SDL_FreeSurface(surface);
}

Entity::~Entity()
{
}
