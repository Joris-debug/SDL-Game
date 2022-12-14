#include "Vicinity.h"

Vicinity::Vicinity(SDL_Surface *surface, SDL_FRect m_bounds_, SDL_Renderer *renderer)
{
	this->m_p_texture_ = SDL_CreateTextureFromSurface(renderer, surface);
	this->m_bounds_ = m_bounds_;
	SDL_FreeSurface(surface);
}

Vicinity::~Vicinity()
{
	SDL_DestroyTexture(m_p_texture_);
}

void Vicinity::moveVicinity(float x, float y)
{
	m_bounds_.x += x;
	m_bounds_.y += y;

}

void Vicinity::renderVicinity(SDL_Renderer* renderer)
{
	SDL_RenderCopyF(renderer, m_p_texture_, NULL, &m_bounds_);	
}
