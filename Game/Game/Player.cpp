#include "Player.h"
#include "SDL_image.h"
#include "Resources.h"
#include <iostream>
Player::Player(SDL_Renderer* renderer)
{
	m_bounds_ = { 400 - 20, 400, 38 * 2, 20 * 2};
	m_spriteBounds_ = { 330, 300 - (80 - 20), 120 * 2, 80 * 2 };
	SDL_Surface *tmpSurface = IMG_Load(RSC_PLAYER_IDLE);
	m_p_textureIdle_ = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
	currentMode = 1; //Player spawns in idle
}

Player::~Player()
{
}

void Player::animatePlayer()
{
	
	int totalFrames = 10;   
	int delayPerFrame = 150;
	int tmp = 120 * ((SDL_GetTicks() / delayPerFrame) % totalFrames);
	currentMode = 1;
	m_spriteCoords_ = { tmp, 0, 120, 80 };
}

void Player::renderPlayer(SDL_Renderer *renderer, double pixel_per_pixel)
{
	SDL_FRect tmp = m_spriteBounds_;
	tmp.x = round(tmp.x * pixel_per_pixel);
	tmp.y = round(tmp.y * pixel_per_pixel);
	tmp.w = round(tmp.w * pixel_per_pixel);
	tmp.h = round(tmp.h * pixel_per_pixel);

	if (currentMode == 1)
		SDL_RenderCopyF(renderer, m_p_textureIdle_, &m_spriteCoords_, &tmp);
}
