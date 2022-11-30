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
	Uint32 m_lastDamageTaken_;	// Timestamp of the last time the Body took damage
	Uint32 m_lastFrame_;		// Timestamp of the last frame
	short m_currentMode_;		// 1: Idle, 2: Walk, 3: Attack, 4: Turn, ...
	short m_currentSprite_;		// The current sprite of the animation that is playing
	short m_currentLives_;
	short m_maxLives_;
public:
	virtual void animateBody(int x, int y) = 0;
	virtual void renderBody(SDL_Renderer* renderer, double pixel_per_pixel) = 0;
	void moveBody(float x, float y);
	void damageBody(short damage);
	inline short getCurrentLives() { return m_currentLives_; }
	bool isInvincible();
	Body(SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_);
	~Body();
};

