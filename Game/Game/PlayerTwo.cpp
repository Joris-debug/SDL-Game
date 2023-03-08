#include "PlayerTwo.h"
#include "Resources.h"
#include "SDL_image.h"
#include <iostream>

PlayerTwo::PlayerTwo(SDL_Renderer* renderer, SDL_FPoint startPos) : Body({ startPos.x,startPos.y, 40, 75 }, { startPos.x - 90, startPos.y - 85, 120 * 2, 80 * 2 }, 3)
{
	SDL_Surface* tmpSurface = IMG_Load(RSC_PLAYER2_IDLE);
	m_p_textureIdle = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER2_RUN);
	m_p_textureRun = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER2_TURN);
	m_p_textureTurn = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER2_ATTACK);
	m_p_textureAttack = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER2_HIT);
	m_p_textureHit = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	m_currentDirection = 0; //Player two spawns looking right
}

PlayerTwo::~PlayerTwo()
{
	SDL_DestroyTexture(m_p_textureAttack);
	SDL_DestroyTexture(m_p_textureTurn);
	SDL_DestroyTexture(m_p_textureIdle);
	SDL_DestroyTexture(m_p_textureRun);
}

void PlayerTwo::renderBody(SDL_Renderer* renderer)
{
	animateBody(0, 0);
	SDL_RendererFlip flip = (m_currentDirection) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	if (m_currentDirection)		//Spritesheet has an offset
		m_spriteBounds.x -= 20;

	switch (m_currentMode) {
	case Mode::idle:
		SDL_RenderCopyExF(renderer, m_p_textureIdle, &m_textureCoords, &m_spriteBounds, NULL, NULL, flip);
		break;
	case Mode::walk:
		SDL_RenderCopyExF(renderer, m_p_textureRun, &m_textureCoords, &m_spriteBounds, NULL, NULL, flip);
		break;
	case Mode::attack:
		SDL_RenderCopyExF(renderer, m_p_textureAttack, &m_textureCoords, &m_spriteBounds, NULL, NULL, flip);
		break;
	case Mode::turn:
		SDL_RenderCopyExF(renderer, m_p_textureTurn, &m_textureCoords, &m_spriteBounds, NULL, NULL, flip);
		break;
	case Mode::hit:
		SDL_RenderCopyExF(renderer, m_p_textureHit, &m_textureCoords, &m_spriteBounds, NULL, NULL, flip);
		break;
	}

	if (m_currentDirection)
		m_spriteBounds.x += 20;

}

void PlayerTwo::teleportPlayerTwo(SDL_FPoint newPos)
{
	m_bounds.x = newPos.x;
	m_bounds.y = newPos.y;
	m_spriteBounds.x = newPos.x - 90;
	m_spriteBounds.y = newPos.y - 85;
}

void PlayerTwo::animateBody(float x, float y)
{
	m_textureCoords = { 120 * m_currentSprite, 0, 120, 80 };
}

SDL_FPoint* PlayerTwo::getPlayerTargets()
{
	return nullptr;
}

