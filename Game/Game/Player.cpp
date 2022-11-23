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

	m_footSpace_ = { 380, 400, 38 * 2, 10 * 2 }; //Todo

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

	if (m_isTurning_) //A turn started less than 3 frames ago
	{
		totalSprites = 3;
		if (m_currentSprite_ == 2)
			m_isTurning_ = false;
	}

	else if (detectTurning(x, y)) {	//A turn starts now
		totalSprites = 3;
		m_currentMode_ = 4;
		m_isTurning_ = true;
		m_lastFrame_ = SDL_GetTicks();
		m_currentSprite_ = 0;
	}

	else if (!x && !y) {
		totalSprites = 10;
		m_currentMode_ = 1;
	}

	else if (x == -1) {		//Walk right
		totalSprites = 10;
		m_currentMode_ = 2;
	}

	else if (x == 1) {		//Walk left
		spriteLayer = 1;
		totalSprites = 10;
		m_currentMode_ = 3;
	}

	else if (y == 1) {		//Walk upwards
		totalSprites = 10;
		m_currentMode_ = 3;
	}

	else if (y == -1) {		//Walk downwards
		spriteLayer = 1;
		totalSprites = 10;
		m_currentMode_ = 2;
	}

	if (m_lastFrame_ + delayPerFrame < SDL_GetTicks()) {
		m_lastFrame_ = SDL_GetTicks();
		m_currentSprite_++;
	}
	
	if (m_currentSprite_ >= totalSprites) {
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
	case 1:
		SDL_RenderCopyF(renderer, m_p_textureIdle_, &m_textureCoords_, &tmp);
		break;
	case 2:
	case 3:
		SDL_RenderCopyF(renderer, m_p_textureRun_, &m_textureCoords_, &tmp);
		break;
	case 4:
		SDL_RenderCopyF(renderer, m_p_textureTurn_, &m_textureCoords_, &tmp);
		break;
	}
}
