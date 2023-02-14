#include "Body.h"
#include <iostream>
void Body::moveEntity(float x, float y)
{
	Entity::moveEntity(x, y);
	m_spriteBounds.x += x;
	m_spriteBounds.y += y;
}

void Body::damageBody(short damage)
{
	if (isInvincible())
		return;

	if (m_currentLives > damage) {
		m_currentLives -= damage;
	}
	else {
		m_currentLives = 0;
	}
	m_p_lastDamageTaken->setStartPoint(SDL_GetTicks());
	m_p_lastFrame->setStartPoint(0); //instantly print a new frame
	m_currentSprite = 0;
}

bool Body::isInvincible()
{

	if (!m_p_lastDamageTaken->checkClockState()) {
		return true;
	}
	else
		return false;
}

Body::Body(SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_) : Entity(m_spriteBounds_)
{
	this->m_bounds = m_bounds_;
	this->m_spriteBounds = m_spriteBounds_;
	this->m_textureCoords = { 0, 0, int(m_spriteBounds_.h) / 2, int(m_spriteBounds_.w) / 2 };
	this->m_maxLives = m_maxLives_;
	m_currentLives = m_maxLives_;
	m_currentMode = Mode::idle; //Player spawns in idle
	m_currentSprite = 1; //The sprite that is currently playing
	m_p_lastFrame = new Clock(100);
	m_p_lastFrame->setStartPoint(0); // trigger new frame
	m_p_lastDamageTaken = new Clock(1000, false); //Every spawned Body can take damage
	m_p_lastDamageTaken->setStartPoint(-1000);
}

Body::~Body()
{
}