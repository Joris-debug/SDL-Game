#pragma once
#include "SDL.h"
#include "Entity.h"
#include "Clock.h"
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
	SDL_FRect m_spriteBounds;
	SDL_Rect m_textureCoords;
	Clock* m_p_lastDamageTaken;	// Timestamp of the last time the Body took damage
	Clock* m_p_lastFrame;		// Timestamp of the last frame
	Mode m_currentMode;		
	short m_currentSprite;		// The current sprite of the animation that is playing
	short m_currentLives;		
	short m_maxLives;		
public:
	virtual void animateBody(float x, float y) = 0;
	virtual void renderBody(SDL_Renderer* renderer) = 0;
	void moveEntity(float x, float y) override;
	void damageBody(short damage);
	inline short getCurrentLives() { return m_currentLives; }
	inline short getMaxLives() { return m_maxLives; }
	inline SDL_FRect* getSpriteBounds() { return &m_spriteBounds; }
	bool isInvincible();
	Body(SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_);
	~Body();
};

