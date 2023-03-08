#include "Player.h"
#include "SDL_image.h"
#include "Resources.h"
#include <iostream>

Player::Player(SDL_Renderer* renderer) : Body({ 380, 285, 40, 75 }, { 290, 200, 120 * 2, 80 * 2 }, 3)
{
	SDL_Surface *tmpSurface = IMG_Load(RSC_PLAYER_IDLE);
	m_p_textureIdle = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER_RUN);
	m_p_textureRun = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER_TURN);
	m_p_textureTurn = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER_ATTACK);
	m_p_textureAttack = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(RSC_PLAYER_HIT);
	m_p_textureHit = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	m_footSpace = { 378, 336, 42, 24}; //This area collides with obstacles 

	m_currentDirection = 0; //Player spawns looking right
	m_lastDirection = 0;

	m_coinCounter = 0;
	m_attackCooldown = PLAYER_ATTACK_COOLDOWN;
	m_p_lastAttack = new Clock(m_attackCooldown, false);
	m_p_lastAttack->setStartPoint(-6000);
	animateBody(0, 0);
}

Player::~Player()
{
	SDL_DestroyTexture(m_p_textureAttack);
	SDL_DestroyTexture(m_p_textureTurn);
	SDL_DestroyTexture(m_p_textureIdle);
	SDL_DestroyTexture(m_p_textureRun);
}

bool Player::detectTurning()
{
	//The player currently has to walk in a certain direction
	if (m_currentMode != Mode::walk)
		return false;
	if(m_lastDirection != m_currentDirection) {
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
			m_currentMode = Mode::hit;
			totalSprites = 2;
			break;
		}

		if (m_currentMode == Mode::attack) //An attack started
		{
			totalSprites = 10;

			if (m_currentSprite >= 9) //Attack ends
				m_currentMode = Mode::idle;

			break;
		}
		if (m_currentMode == Mode::turn) //A turn started less than 3 frames ago
		{
			totalSprites = 3;
			if (m_currentSprite >= 2)
				m_currentMode = Mode::idle;
			break;
		}

		if (!x && !y) {	//Idle
			totalSprites = 10;
			m_currentMode = Mode::idle;
			break;
		}

		m_currentDirection = (x > 0 || (y < 0 && x >= 0));

		if (detectTurning()) {	//A turn movement starts now
			totalSprites = 3;
			m_currentMode = Mode::turn;
			m_p_lastFrame->setStartPoint(0);
			m_currentSprite = 0;
			break;
		}

		if (m_currentDirection == 0) {		//Walk right or upwards
			totalSprites = 10;
			m_currentMode = Mode::walk;
			break;
		}

		if (m_currentDirection == 1) {		//Walk left or downwards
			totalSprites = 10;
			m_currentMode = Mode::walk;
			break;
		}

	} while (false);


	if (m_p_lastFrame->checkClockState()) {	//Next sprite
		m_currentSprite++;
	}
	
	if (m_currentSprite >= totalSprites) {		//End of spritesheet
		m_currentSprite = 0;
	}

	m_lastDirection = m_currentDirection;

	m_textureCoords = { 120 * m_currentSprite, 0, 120, 80 };
}

void Player::renderBody(SDL_Renderer* renderer)
{
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

float Player::getAttackCooldownPercent()
{
	if (checkAttackCooldown())
		return 1.0f;

	Uint32 lastAttack = m_p_lastAttack->getStartPoint();
	return (SDL_GetTicks() - lastAttack) / float(m_attackCooldown);
}

SDL_FPoint* Player::getPlayerTargets()
{
	static SDL_FPoint s_playerTargets[3];
	s_playerTargets[0] = { m_bounds.x + m_bounds.w / 2, m_bounds.y + m_bounds.h / 2 }; //Middle of the Player
	s_playerTargets[1] = { m_bounds.x + m_bounds.w / 2, m_bounds.y }; //Head of the Player
	s_playerTargets[2] = { m_bounds.x + m_bounds.w / 2, m_bounds.y + m_bounds.h }; //Feet of the Player
	return s_playerTargets;
}
