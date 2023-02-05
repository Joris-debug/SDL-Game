#include "Vicinity.h"

Vicinity::Vicinity(SDL_Surface *surface, SDL_FRect m_bounds_, SDL_Renderer *renderer)
{
	this->m_p_texture = SDL_CreateTextureFromSurface(renderer, surface);
	this->m_bounds = m_bounds_;
	SDL_FreeSurface(surface);
}

Vicinity::~Vicinity()
{
	SDL_DestroyTexture(m_p_texture);
}

void Vicinity::moveVicinity(float x, float y)
{
	m_bounds.x += x;
	m_bounds.y += y;

}

void Vicinity::renderVicinity(SDL_Renderer* renderer)
{
	SDL_RenderCopyF(renderer, m_p_texture, NULL, &m_bounds);	
}
