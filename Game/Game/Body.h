#pragma once
#include "SDL.h"
#include "Entity.h"

struct walkingVector {
	int x = 0;
	int y = 0;
};

class Body :
	public Entity
{
protected:
	SDL_FRect m_spriteBounds_;
	SDL_Rect m_textureCoords_;
	walkingVector m_lastMove_;
	Uint32 m_lastFrame_;
	int m_currentMode_;		// 1: Idle, 2: Walk left, 3: Walk right, ...
	int m_currentSprite_;	// The current sprite of the animation that is playing
public:
	virtual void animateBody(int x, int y) = 0;
	virtual void renderBody(SDL_Renderer* renderer, double pixel_per_pixel) = 0;
	void moveBody(float x, float y);
	Body(SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_);
	~Body();
};

