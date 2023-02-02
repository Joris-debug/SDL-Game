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

	m_currentDirection_ = 0; //Player spawns looking right
	m_lastDirection_ = 0;

	m_isTurning_ = false;
	m_isAttacking_ = false;
	m_coinCounter_ = 0;
	m_attackCooldown_ = PLAYER_ATTACK_COOLDOWN;
	m_p_lastAttack_ = new Clock(m_attackCooldown_, false);
}

Player::~Player()
{
	SDL_DestroyTexture(m_p_textureAttack_);
	SDL_DestroyTexture(m_p_textureTurn_);
	SDL_DestroyTexture(m_p_textureIdle_);
	SDL_DestroyTexture(m_p_textureRun_);
}

bool Player::detectTurning()
{
	//The player currently has to walk in a certain direction
	if (m_currentMode_ != Mode::walk)
		return false;
	if(m_lastDirection_ != m_currentDirection_) {
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

			if (m_currentSprite_ >= 9) //Attack ends
				m_isAttacking_ = false;

			break;
		}
		if (m_isTurning_) //A turn started less than 3 frames ago
		{
			totalSprites = 3;
			if (m_currentSprite_ >= 2)
				m_isTurning_ = false;
			break;
		}

		if (!x && !y) {	//Idle
			totalSprites = 10;
			m_currentMode_ = Mode::idle;
			break;
		}

		m_currentDirection_ = (x > 0 || (y < 0 && x >= 0));

		if (detectTurning()) {	//A turn movement starts now
			totalSprites = 3;
			m_currentMode_ = Mode::turn;
			m_isTurning_ = true;
			m_p_lastFrame_->setStartPoint(0);
			m_currentSprite_ = 0;
			break;
		}

		if (m_currentDirection_ == 0) {		//Walk right or upwards
			totalSprites = 10;
			m_currentMode_ = Mode::walk;
			break;
		}

		if (m_currentDirection_ == 1) {		//Walk left or downwards
			totalSprites = 10;
			m_currentMode_ = Mode::walk;
			break;
		}

	} while (false);


	if (m_p_lastFrame_->checkClockState()) {	//Next sprite
		m_currentSprite_++;
	}
	
	if (m_currentSprite_ >= totalSprites) {		//End of spritesheet
		m_currentSprite_ = 0;
	}

	m_lastDirection_ = m_currentDirection_;

	m_textureCoords_ = { 120 * m_currentSprite_, 0, 120, 80 };
}

void Player::renderBody(SDL_Renderer* renderer)
{
	SDL_RendererFlip flip = (m_currentDirection_) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	switch (m_currentMode_) {
	case Mode::idle:
		SDL_RenderCopyExF(renderer, m_p_textureIdle_, &m_textureCoords_, &m_spriteBounds_, NULL, NULL, flip);
		break;
	case Mode::walk:
		SDL_RenderCopyExF(renderer, m_p_textureRun_, &m_textureCoords_, &m_spriteBounds_, NULL, NULL, flip);
		break;
	case Mode::attack:
		SDL_RenderCopyExF(renderer, m_p_textureAttack_, &m_textureCoords_, &m_spriteBounds_, NULL, NULL, flip);
		break;
	case Mode::turn:
		SDL_RenderCopyExF(renderer, m_p_textureTurn_, &m_textureCoords_, &m_spriteBounds_, NULL, NULL, flip);
		break;
	case Mode::hit:
		SDL_RenderCopyExF(renderer, m_p_textureHit_, &m_textureCoords_, &m_spriteBounds_, NULL, NULL, flip);
		break;
	}
}

float Player::getAttackCooldownPercent()
{
	if (checkAttackCooldown())
		return 1.0f;

	Uint32 lastAttack = m_p_lastAttack_->getStartPoint();
	return (SDL_GetTicks() - lastAttack) / PLAYER_ATTACK_COOLDOWN;
}

SDL_FPoint* Player::getPlayerTargets()
{
	static SDL_FPoint s_playerTargets[3];
	s_playerTargets[0] = { m_bounds_.x + m_bounds_.w / 2, m_bounds_.y + m_bounds_.h / 2 }; //Middle of the Player
	s_playerTargets[1] = { m_bounds_.x + m_bounds_.w / 2, m_bounds_.y }; //Head of the Player
	s_playerTargets[2] = { m_bounds_.x + m_bounds_.w / 2, m_bounds_.y + m_bounds_.h }; //Feet of the Player
	return s_playerTargets;
}
