#pragma once
#include "SDL.h"

class Entity
{
protected:
	SDL_FRect m_bounds_;
public:
	inline SDL_FRect* getBounds() { return &m_bounds_; }
	inline virtual void moveEntity(float x, float y) { m_bounds_.x += x; m_bounds_.y += y; }
	Entity(SDL_FRect m_bounds_);
	~Entity();
};

