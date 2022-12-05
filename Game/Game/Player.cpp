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
	m_footSpace_ = { 378, 336, 42, 24}; //This area collides with obstacles 
	m_isTurning_ = false;
	m_isAttacking_ = false;
}

Player::~Player()
{
	SDL_DestroyTexture(m_p_textureAttack_);
	SDL_DestroyTexture(m_p_textureTurn_);
	SDL_DestroyTexture(m_p_textureIdle_);
	SDL_DestroyTexture(m_p_textureRun_);
}

bool Player::detectTurning(int x, int y)
{
	if ((m_lastMove_.y == -1 || m_lastMove_.x == 1) && x == 0 && y == 0) {
		return true;
	}

	return false;
}

void Player::animateBody(int x, int y)
{	
	int totalSprites = 1;
	int delayPerFrame = 100;
	int spriteLayer = 0;

	do {

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
			m_lastFrame_ = SDL_GetTicks();
			m_currentSprite_ = 0;
			break;
		}

		if (!x && !y) {	//Idle
			totalSprites = 10;
			m_currentMode_ = Mode::idle;
			break;
		}

		if (x == -1 || (y == 1 && x != 1)) {		//Walk right or upwards
			totalSprites = 10;
			m_currentMode_ = Mode::walk;
			break;
		}

		if (x == 1 || (y == -1 && x != -1)) {		//Walk left or downwards
			spriteLayer = 1;
			totalSprites = 10;
			m_currentMode_ = Mode::walk;
			break;
		}


	} while (false);


	if (m_lastFrame_ + delayPerFrame < SDL_GetTicks()) {	//Next sprite
		m_lastFrame_ = SDL_GetTicks();
		m_currentSprite_++;
	}
	
	if (m_currentSprite_ >= totalSprites) {		//End of spritesheet
		m_currentSprite_ = 1;
	}

	m_lastMove_.x = x;
	m_lastMove_.y = y;

	m_textureCoords_ = { 120 * m_currentSprite_, spriteLayer * 80, 120, 80 };
}

void Player::renderBody(SDL_Renderer* renderer, double pixel_per_pixel)
{
	SDL_FRect tmp = m_spriteBounds_;
	tmp.x = round(tmp.x * pixel_per_pixel);
	tmp.y = round(tmp.y * pixel_per_pixel);
	tmp.w = round(tmp.w * pixel_per_pixel);
	tmp.h = round(tmp.h * pixel_per_pixel);

	switch (m_currentMode_) {
	case Mode::idle:
		SDL_RenderCopyF(renderer, m_p_textureIdle_, &m_textureCoords_, &tmp);
		break;
	case Mode::walk:
		SDL_RenderCopyF(renderer, m_p_textureRun_, &m_textureCoords_, &tmp);
		break;
	case Mode::attack:
		SDL_RenderCopyF(renderer, m_p_textureAttack_, &m_textureCoords_, &tmp);
		break;
	case Mode::turn:
		SDL_RenderCopyF(renderer, m_p_textureTurn_, &m_textureCoords_, &tmp);
		break;
	}
}
