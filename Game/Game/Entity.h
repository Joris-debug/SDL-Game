#pragma once
#include "SDL.h"

class Entity
{
protected:
	SDL_FRect m_bounds_;
public:
	inline SDL_FRect getBounds() { return m_bounds_; }
	Entity(SDL_FRect m_bounds_);
	~Entity();
};

