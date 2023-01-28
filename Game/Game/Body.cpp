#include "Body.h"
#include <iostream>
void Body::moveEntity(float x, float y)
{
	Entity::moveEntity(x, y);
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
	m_lastFrame_.setStartPoint(0); //instantly print a new frame
	m_currentSprite_ = 0;
}

bool Body::isInvincible()
{

	if (m_lastDamageTaken_ + 1000 >= SDL_GetTicks()) {
		return true;
	}
	else
		return false;
}

Body::Body(SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_) : Entity(m_spriteBounds_)
{
	this->m_bounds_ = m_bounds_;
	this->m_spriteBounds_ = m_spriteBounds_;
	this->m_textureCoords_ = { 0, 0, int(m_spriteBounds_.h) / 2, int(m_spriteBounds_.w) / 2 };
	this->m_maxLives_ = m_maxLives_;
	m_currentLives_ = m_maxLives_;
	m_lastFrame_.setStartPoint(0); // trigger new frame
	m_lastFrame_.setInterval(100);
	m_currentMode_ = Mode::idle; //Player spawns in idle
	m_currentSprite_ = 1; //The sprite that is currently playing
	m_lastDamageTaken_ = 0; //Every spawned Body can take damage

}

Body::~Body()
{
}