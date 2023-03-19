#include "PlayerTwo.h"
#include "Resources.h"
#include "SDL_image.h"
#include "Clock.h"
#include <iostream>

PlayerTwo::PlayerTwo(bool m_playerType, SDL_Renderer* renderer, SDL_FPoint startPos) : Body({ startPos.x,startPos.y, 40, 75 }, { startPos.x - 90, startPos.y - 85, 120 * 2, 80 * 2 }, 3)
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
	m_hitDetected = false;
	this->m_playerType = m_playerType;
	m_p_lastAttackTrigger = new Clock(1000, false);
	m_p_lastAttackTrigger->setStartPoint(-1000);
	m_footSpace = { startPos.x, startPos.y + 50, 40, 24 };
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
	m_footSpace.x = newPos.x;
	m_footSpace.y = newPos.y + 50;
}

void PlayerTwo::animateBody(float x, float y)
{
	m_textureCoords = { 120 * m_currentSprite, 0, 120, 80 };
}

bool PlayerTwo::damageBody(short damage)
{
	if (m_currentMode == Mode::hit)
		return false;

	return true;
}

void PlayerTwo::moveEntity(float x, float y)
{
	moveFootSpace(x, y);
	Body::moveEntity(x, y);
}

SDL_FRect* PlayerTwo::getPlayerTwoScreen()
{
	static SDL_FRect screenBouds{ 0, 0, 800, 640 };
	screenBouds.x = m_spriteBounds.x - 290;
	screenBouds.x = m_spriteBounds.y - 200;
	return &screenBouds;
}

SDL_FPoint* PlayerTwo::getPlayerTwoTargets()
{
	static SDL_FPoint s_playerTargets[3];
	s_playerTargets[0] = { m_bounds.x + m_bounds.w / 2, m_bounds.y + m_bounds.h / 2 }; //Middle of the Player
	s_playerTargets[1] = { m_bounds.x + m_bounds.w / 2, m_bounds.y }; //Head of the Player
	s_playerTargets[2] = { m_bounds.x + m_bounds.w / 2, m_bounds.y + m_bounds.h }; //Feet of the Player
	return s_playerTargets;
}

