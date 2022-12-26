#pragma once
#include "SDL.h"
#include "Entity.h"

enum class Mode{idle, walk, attack, turn, hit};	// 0: Idle, 1: Walk, 2: Attack, 3: Turn, 4: Hit, ...

struct walkingVector {
	float x = 0;
	float y = 0;
	bool operator ==(walkingVector a) {
		if ((a.x == this->x) && (a.y == this->y))
			return true;
		return false;
	}
	bool operator <=(int a) {
		if ((a >= this->x) && (a >= this->y))
			return true;
		return false;
	}
	bool operator >=(int a) {
		if ((a <= this->x) && (a <= this->y))
			return true;
		return false;
	}
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
	Mode m_currentMode_;		
	short m_currentSprite_;		// The current sprite of the animation that is playing
	short m_currentLives_;		
	short m_maxLives_;		
public:
	virtual void animateBody(float x, float y) = 0;
	virtual void renderBody(SDL_Renderer* renderer) = 0;
	void moveEntity(float x, float y) override;
	void damageBody(short damage);
	inline short getCurrentLives() { return m_currentLives_; }
	inline short getMaxLives() { return m_maxLives_; }
	inline SDL_FRect* getSpriteBounds() { return &m_spriteBounds_; }
	bool isInvincible();
	Body(SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_);
	~Body();
};

