#include "Vicinity.h"

Vicinity::Vicinity(SDL_Surface *surface, SDL_Rect m_bounds_, SDL_Renderer *renderer)
{
	this->m_p_texture_ = SDL_CreateTextureFromSurface(renderer, surface);
	this->m_bounds_ = m_bounds_;
	SDL_FreeSurface(surface);
}

Vicinity::~Vicinity()
{
	SDL_DestroyTexture(m_p_texture_);
}

void Vicinity::moveVicinty(int x, int y)
{
	m_bounds_.x += x;
	m_bounds_.y += y;
}

void Vicinity::renderVicinity(SDL_Renderer* renderer, double pixel_per_pixel)
{
	SDL_Rect tmp = m_bounds_;
	tmp.x *= pixel_per_pixel;
	tmp.y*= pixel_per_pixel;
	tmp.w *= pixel_per_pixel;
	tmp.h *= pixel_per_pixel;
	SDL_RenderCopy(renderer, m_p_texture_, NULL, &tmp);
}
