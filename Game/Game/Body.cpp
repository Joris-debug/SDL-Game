#include "Body.h"

Body::Body(SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_) : Entity(m_spriteBounds_)
{
	this->m_bounds_ = m_bounds_;
	this->m_spriteBounds_ = m_spriteBounds_;
	this->m_spriteCoords_ = { 0, 0, 0, 0 }; //Has yet to be assigned
	m_lastFrame_ = 0;// trigger new frame
	m_currentMode_ = 1; //Player spawns in idle
	m_currentSprite_ = 1; //The sprite that is currently playing

}

Body::~Body()
{
}
