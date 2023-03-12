#pragma once
#include "SDL.h"

class Entity
{
protected:
	SDL_FRect m_bounds;
public:
	inline SDL_FRect* getBounds() { return &m_bounds; }
	inline virtual void moveEntity(float x, float y) { m_bounds.x += x; m_bounds.y += y; }
	Entity(SDL_FRect m_bounds_);
	virtual ~Entity();
};

