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

Vicinity::Vicinity()
{
}

void Vicinity::moveVicinity(float x, float y)
{
	m_bounds_.x += x;
	m_bounds_.y += y;

}

void Vicinity::renderVicinity(SDL_Renderer* renderer, double pixel_per_pixel, int screenWidth)
{
	SDL_FRect tmp = m_bounds_;
	tmp.x = round(tmp.x * pixel_per_pixel);
	tmp.y = round(tmp.y * pixel_per_pixel);
	tmp.w = round(tmp.w * pixel_per_pixel);
	tmp.h = round(tmp.h * pixel_per_pixel);
	SDL_RenderCopyF(renderer, m_p_texture_, NULL, &tmp);


	while(tmp.x + tmp.w < screenWidth)
	{ 
		tmp.x += tmp.w;
		SDL_RenderCopyF(renderer, m_p_texture_, NULL, &tmp);
	}

	
	
}
