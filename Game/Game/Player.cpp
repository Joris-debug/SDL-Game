#include "Player.h"
#include "SDL_image.h"
#include "Resources.h"
#include <iostream>
Player::Player(SDL_Renderer* renderer) : Body({ 380, 285, 40, 75 }, { 290, 200, 120 * 2, 80 * 2 }, 3)
{
	SDL_Surface *tmpSurface = IMG_Load(RSC_PLAYER_IDLE);
	m_p_textureIdle_ = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER_RUN);
	m_p_textureRun_ = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER_TURN);
	m_p_textureTurn_ = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER_ATTACK);
	m_p_textureAttack_ = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER_HIT);
	m_p_textureHit_ = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	m_footSpace_ = { 378, 336, 42, 24}; //This area collides with obstacles 
	m_isTurning_ = false;
	m_isAttacking_ = false;
	m_coinCounter_ = 0;
	m_lastAttack_ = SDL_GetTicks();
}

Player::~Player()
{
	SDL_DestroyTexture(m_p_textureAttack_);
	SDL_DestroyTexture(m_p_textureTurn_);
	SDL_DestroyTexture(m_p_textureIdle_);
	SDL_DestroyTexture(m_p_textureRun_);
}

bool Player::detectTurning(float x, float y)
{
	if ((m_lastMove_.y == -1 || m_lastMove_.x == 1) && x == 0 && y == 0) {
		return true;
	}

	return false;
}

void Player::animateBody(float x, float y)
{	
	int totalSprites = 1;
	int delayPerFrame = 100;
	int spriteLayer = 0;

	do {
		if (isInvincible()) {
			m_currentMode_ = Mode::hit;
			totalSprites = 2;
			m_isAttacking_ = false;
			break;
		}

		if (m_isAttacking_) //An attack started
		{
			totalSprites = 10;
			m_currentMode_ = Mode::attack;
			if (m_lastMove_.x == 1 || (m_lastMove_.y == -1 && m_lastMove_.x != -1))
				spriteLayer = 1;
			if (m_currentSprite_ >= 9) //Attack ends
				m_isAttacking_ = false;

			x = m_lastMove_.x;
			y = m_lastMove_.y;
			break;
		}
		if (m_isTurning_) //A turn started less than 3 frames ago
		{
			totalSprites = 3;
			if (m_currentSprite_ >= 2)
				m_isTurning_ = false;
			break;
		}

		if (detectTurning(x, y)) {	//A turn movement starts now
			totalSprites = 3;
			m_currentMode_ = Mode::turn;
			m_isTurning_ = true;
			m_lastFrame_.setStartPoint(0);
			m_currentSprite_ = 0;
			break;
		}

		if (!x && !y) {	//Idle
			totalSprites = 10;
			m_currentMode_ = Mode::idle;
			break;
		}

		if (x < 0 || (y > 0 && x <= 0)) {		//Walk right or upwards
			totalSprites = 10;
			m_currentMode_ = Mode::walk;
			break;
		}

		if (x > 0 || (y < 0 && x >= 0)) {		//Walk left or downwards
			spriteLayer = 1;
			totalSprites = 10;
			m_currentMode_ = Mode::walk;
			break;
		}


	} while (false);


	if (m_lastFrame_ .checkClockState()) {	//Next sprite
		m_currentSprite_++;
	}
	
	if (m_currentSprite_ >= totalSprites) {		//End of spritesheet
		m_currentSprite_ = 0;
	}

	m_lastMove_.x = x;
	m_lastMove_.y = y;

	m_textureCoords_ = { 120 * m_currentSprite_, spriteLayer * 80, 120, 80 };
}

void Player::renderBody(SDL_Renderer* renderer)
{
	switch (m_currentMode_) {
	case Mode::idle:
		SDL_RenderCopyF(renderer, m_p_textureIdle_, &m_textureCoords_, &m_spriteBounds_);
		break;
	case Mode::walk:
		SDL_RenderCopyF(renderer, m_p_textureRun_, &m_textureCoords_, &m_spriteBounds_);
		break;
	case Mode::attack:
		SDL_RenderCopyF(renderer, m_p_textureAttack_, &m_textureCoords_, &m_spriteBounds_);
		break;
	case Mode::turn:
		SDL_RenderCopyF(renderer, m_p_textureTurn_, &m_textureCoords_, &m_spriteBounds_);
		break;
	case Mode::hit:
		SDL_RenderCopyF(renderer, m_p_textureHit_, &m_textureCoords_, &m_spriteBounds_);
		break;
	}
}

SDL_FPoint* Player::getPlayerTargets()
{
	static SDL_FPoint s_playerTargets[3];
	s_playerTargets[0] = { m_bounds_.x + m_bounds_.w / 2, m_bounds_.y + m_bounds_.h / 2 }; //Middle of the Player
	s_playerTargets[1] = { m_bounds_.x + m_bounds_.w / 2, m_bounds_.y }; //Head of the Player
	s_playerTargets[2] = { m_bounds_.x + m_bounds_.w / 2, m_bounds_.y + m_bounds_.h }; //Feet of the Player
	return s_playerTargets;
}
