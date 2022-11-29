#include "Player.h"
#include "SDL_image.h"
#include "Resources.h"
#include <iostream>
Player::Player(SDL_Renderer* renderer) : Body({ 380, 285, 40, 75 }, { 290, 200, 120 * 2, 80 * 2 })
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

	m_footSpace_ = { 378, 336, 42, 24}; //Todo
	m_playerLives_ = 3;
	m_isTurning_ = false;
}

Player::~Player()
{
	//todo
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
			m_currentMode_ = 3;
			if (m_lastMove_.x == 1 || (m_lastMove_.y == -1 && m_lastMove_.x != -1))
				spriteLayer = 1;
			if (m_currentSprite_ >= 9)
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
			m_currentMode_ = 4;
			m_isTurning_ = true;
			m_lastFrame_ = SDL_GetTicks();
			m_currentSprite_ = 0;
			break;
		}

		if (!x && !y) {	//Idle
			totalSprites = 10;
			m_currentMode_ = 1;
			break;
		}

		if (x == -1 || (y == 1 && x != 1)) {		//Walk right or upwards
			totalSprites = 10;
			m_currentMode_ = 2;
			break;
		}

		if (x == 1 || (y == -1 && x != -1)) {		//Walk left or downwards
			spriteLayer = 1;
			totalSprites = 10;
			m_currentMode_ = 2;
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

void Player::attack(std::list<std::unique_ptr<Enemy>>* entityList)
{
	if (m_isAttacking_) //Player is already attacking
		return;
	m_isAttacking_ = true;
	m_currentSprite_ = 0;
	

}

void Player::renderBody(SDL_Renderer* renderer, double pixel_per_pixel)
{
	SDL_FRect tmp = m_spriteBounds_;
	tmp.x = round(tmp.x * pixel_per_pixel);
	tmp.y = round(tmp.y * pixel_per_pixel);
	tmp.w = round(tmp.w * pixel_per_pixel);
	tmp.h = round(tmp.h * pixel_per_pixel);

	switch (m_currentMode_) {
	case 1:
		SDL_RenderCopyF(renderer, m_p_textureIdle_, &m_textureCoords_, &tmp);
		break;
	case 2:
		SDL_RenderCopyF(renderer, m_p_textureRun_, &m_textureCoords_, &tmp);
		break;
	case 3:
		SDL_RenderCopyF(renderer, m_p_textureAttack_, &m_textureCoords_, &tmp);
		break;
	case 4:
		SDL_RenderCopyF(renderer, m_p_textureTurn_, &m_textureCoords_, &tmp);
		break;
	}
}
