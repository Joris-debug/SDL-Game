#include "Body.h"

void Body::moveBody(float x, float y)
{
	moveEntity(x, y);
	m_spriteBounds_.x += x;
	m_spriteBounds_.y += y;
}

void Body::damageBody(short damage)
{
	if (isInvincible())
		return;

	if (m_currentLives_ > damage) {
		m_currentLives_ -= damage;
	}
	else {
		m_currentLives_ = 0;
	}
	m_lastDamageTaken_ = SDL_GetTicks();
}

bool Body::isInvincible()
{
	if (SDL_GetTicks() - m_lastDamageTaken_ >=  100)
		return false;
	else
		return true;
}

Body::Body(SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_) : Entity(m_spriteBounds_)
{
	this->m_bounds_ = m_bounds_;
	this->m_spriteBounds_ = m_spriteBounds_;
	this->m_textureCoords_ = { 0, 0, 0, 0 }; //Has yet to be assigned
	this->m_maxLives_ = m_maxLives_;
	m_currentLives_ = m_maxLives_;
	m_lastFrame_ = 0; // trigger new frame
	m_currentMode_ = 1; //Player spawns in idle
	m_currentSprite_ = 1; //The sprite that is currently playing
	m_lastDamageTaken_ = 0; //Every spawned Body can take damage

}

Body::~Body()
{
}
