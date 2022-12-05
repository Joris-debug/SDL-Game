#include "Enemy.h"
#include "World.h"
#include "Player.h"
Enemy::Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_) : Body(m_bounds_, m_spriteBounds_, m_maxLives_)
{
	this->m_p_textureIdle_ = m_p_textureIdle_;
	this->m_p_textureRun_ = m_p_textureRun_;
	this->m_p_textureHit_ = m_p_textureHit_;
}

walkingVector Enemy::enemyPathfinding(World* p_world, float deltaTime)
{
	Player* enemyTarget = p_world->getPlayer()->get();
	SDL_FRect targetLocation = *(enemyTarget->getBounds());

	if (isInvincible()) {	//Hurt Enemies don't move
		return { 0, 0 };
	}

	int x = 0, y = 0;
	int margin = 8;

	if (targetLocation.x + (targetLocation.w / 2.0) > m_bounds_.x + (m_bounds_.w / 2.0) + margin)
		x = 1;
	else if (targetLocation.x + (targetLocation.w / 2.0) + margin < m_bounds_.x + (m_bounds_.w / 2.0))
		x = -1;

	if (targetLocation.y + (targetLocation.h / 2.0) > m_bounds_.y + (m_bounds_.h / 2.0) + margin)
		y = 1;
	else if (targetLocation.y + (targetLocation.h / 2.0) + margin < m_bounds_.y + (m_bounds_.h / 2.0))
		y = -1;

	bool xCollision = true, yCollision = true;
	float factor = (unsigned int)this % 5;
	factor += 3;
	float xMovement = (x * deltaTime) * (factor / 10), yMovement = (y * deltaTime) * (factor / 10);
	
	if (x != 0) {
		xCollision = false;
		this->moveEntity(xMovement, 0);

		for (auto const& cursor : *p_world->getEnemyVector()) {
			if (SDL_HasIntersectionF(&m_bounds_, cursor->getBounds()) && this != cursor.get()) {
				xCollision = true;
				break;
			}
		}

		if (!xCollision) {
			for (auto const& cursor : *p_world->getEntityVector()) {
				if (SDL_HasIntersectionF(&m_bounds_, cursor->getBounds()) && this != cursor.get()) {
					xCollision = true;
					break;
				}
			}
		}

		this->moveEntity(-xMovement, 0);
	}

	if (y != 0) {
		this->moveEntity(0, yMovement);
		yCollision = false;
		for (auto const& cursor : *p_world->getEnemyVector()) {
			if (SDL_HasIntersectionF(&m_bounds_, cursor->getBounds()) && this != cursor.get()) {
				yCollision = true;
				break;
			}
		}

		if (!yCollision) {
			for (auto const& cursor : *p_world->getEntityVector()) {
				if (SDL_HasIntersectionF(&m_bounds_, cursor->getBounds()) && this != cursor.get()) {
					yCollision = true;
					break;
				}
			}
		}

		this->moveEntity(0 , -yMovement);
	}

	if (!xCollision && !yCollision && x != 0 && y != 0) { //check if x and y movement results in colission

		this->moveEntity(xMovement, yMovement);
		for (auto const& cursor : *p_world->getEnemyVector()) {
			if (SDL_HasIntersectionF(&m_bounds_, cursor->getBounds()) && this != cursor.get()) {
				yCollision = true;
				break;
			}
		}
		if (!yCollision) {
			for (auto const& cursor : *p_world->getEntityVector()) {
				if (SDL_HasIntersectionF(&m_bounds_, cursor->getBounds()) && this != cursor.get()) {
					yCollision = true;
					break;
				}
			}
		}
		this->moveEntity(-xMovement, -yMovement);
	}

	this->moveBody((x * deltaTime * !xCollision) * (factor / 10), (y * deltaTime * !yCollision) * (factor / 10));

	return { x , y };
}

void Enemy::animateBody(int x, int y)
{
	int totalSprites = 1;
	int delayPerFrame = 100;
	int spriteLayer = 0;

 	do {
		if (isInvincible()) {
			m_currentMode_ = Mode::hit;
			totalSprites = 6;
			break;
		}

		if (!x && !y) {
			delayPerFrame = 6000;
			m_currentMode_ = Mode::idle;
			totalSprites = 4;
			break;
		}

		if (x == 1) {
			m_currentMode_ = Mode::walk;
			totalSprites = 4;
			spriteLayer = 1;
			break;
		}

		if (x == -1) {
			m_currentMode_ = Mode::walk;
			totalSprites = 4;
			spriteLayer = 2;
			break;
		}

		if (y == 1) {
			m_currentMode_ = Mode::walk;
			totalSprites = 4;
			spriteLayer = 0;
			break;
		}

		if (y == -1) {
			m_currentMode_ = Mode::walk;
			totalSprites = 4;
			spriteLayer = 3;
			break;
		}

	} while (false);

	if (m_lastFrame_ + delayPerFrame < SDL_GetTicks()) {	//Next sprite
		m_lastFrame_ = SDL_GetTicks();
		m_currentSprite_++;
	}

	if (m_currentSprite_ >= totalSprites) {		//End of spritesheet
		m_currentSprite_ = 0;
	}

	m_lastMove_.x = x;
	m_lastMove_.y = y;
	m_textureCoords_.x = m_textureCoords_.w * m_currentSprite_;
	m_textureCoords_.y = m_textureCoords_.h * spriteLayer;

}

void Enemy::renderBody(SDL_Renderer* renderer, double pixel_per_pixel)
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
	case Mode::hit:
		SDL_RenderCopyF(renderer, m_p_textureHit_, &m_textureCoords_, &tmp);
		break;
	}
}

Enemy::~Enemy()
{
	std::cout << "Enemy deleted" << std::endl;
}
