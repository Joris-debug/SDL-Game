#pragma once
#include "SDL.h"
class Entity
{
private:
	SDL_Texture* m_p_texture_;
	SDL_FRect m_bounds_;
public:
	Entity(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer);
	~Entity();
	inline SDL_FRect getBounds() { return m_bounds_; }
	inline SDL_Texture* getTexture() { return m_p_texture_; }
};

