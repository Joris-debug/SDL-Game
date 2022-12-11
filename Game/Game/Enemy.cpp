#include "Enemy.h"
#include "World.h"
#include "Player.h"
#include "Resources.h"
Enemy::Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_) : Body(m_bounds_, m_spriteBounds_, m_maxLives_)
{
	this->m_p_textureIdle_ = m_p_textureIdle_;
	this->m_p_textureRun_ = m_p_textureRun_;
	this->m_p_textureHit_ = m_p_textureHit_;
	m_factor_ = (unsigned int)this % 5 + 3;	//Random number for certain calculations
	std::cout << m_factor_ << std::endl;
	m_enemyTarget_ = { m_bounds_.x + m_bounds_.w / 2 , m_bounds_.y + m_bounds_.h / 2 }; //His first target is himself, on the next run of the enemyPathfinding method, a new target should be assigned
}

void Enemy::enemyPathfinding(World* p_world, float deltaTime)
{
	if (isInvincible()) {	//Hurt Enemies don't move
		this->animateBody(0, 0);
		return;
	}
	//----------------------------------------------------------------------------------------------- Checking if the player has been spotted
	
	SDL_FPoint* p_playerTargets = p_world->getPlayer()->get()->getPlayerTargets();
	SDL_FPoint enemyMiddle = { m_bounds_.x + m_bounds_.w / 2, m_bounds_.y + m_bounds_.h / 2 };

	bool playerSpotted = true;
	auto p_entityVector = p_world->getEntityVector();
	auto it = p_entityVector->begin();
	while (it != p_entityVector->end()) {
		if (SDL_IntersectFRectAndLine(it->get()->getBounds(), &p_playerTargets[0].x, &p_playerTargets[0].y, &enemyMiddle.x, &enemyMiddle.y)) {
			playerSpotted = false;
			std::cout << "lol" << std::endl;
		}			
		it++;
	}
	if (playerSpotted == true) {
		std::cout << "player spotted" << std::endl;
	}
	else 
		std::cout << "player not spotted" << std::endl;
	short margin = 8;
	if (abs(enemyMiddle.x - m_enemyTarget_.x) < margin && abs(enemyMiddle.y - m_enemyTarget_.y) < margin) {		//New direction assigned if the old target is reached
		m_enemyTarget_ = p_world->getRandomCoordinate();
	}
	else if (p_world->getRandomNumber(0, 200) == Uint32(this) % 10) {			//New direction assigned if a random check is hit
		m_enemyTarget_ = p_world->getRandomCoordinate();
	}

	float dirX = m_enemyTarget_.x - enemyMiddle.x;
	float dirY = m_enemyTarget_.y - enemyMiddle.y;
	float hyp = sqrt(dirX * dirX + dirY * dirY);

	dirX /= hyp;
	dirY /= hyp;	
	walkingVector legalMove = this->checkEnemyMove(p_world, dirX, dirY, deltaTime);
	walkingVector empty = { 0, 0 };
	if (legalMove == empty) {	//If the enemy stopped before reaching the target
		m_enemyTarget_ = { m_bounds_.x + m_bounds_.w / 2 , m_bounds_.y + m_bounds_.h / 2 };	// On the next run of the enemyPathfinding method, a new target should be assigned
	}
	Body::moveEntity((legalMove.x * deltaTime) * ENEMY_SPEED, (deltaTime * legalMove.y) * ENEMY_SPEED); //This method does not move the enemy target
	animateBody(dirX, dirY);
}

void Enemy::animateBody(float x, float y)
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

		if (abs(x) > abs(y)) {

			if (x > 0) {
				m_currentMode_ = Mode::walk;
				totalSprites = 4;
				spriteLayer = 1;
				break;
			}

			if (x < 0) {
				m_currentMode_ = Mode::walk;
				totalSprites = 4;
				spriteLayer = 2;
				break;
			}
		}
		else {

			if (y > 0) {
				m_currentMode_ = Mode::walk;
				totalSprites = 4;
				spriteLayer = 0;
				break;
			}

			if (y < 0) {
				m_currentMode_ = Mode::walk;
				totalSprites = 4;
				spriteLayer = 3;
				break;
			}
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

void Enemy::moveEntity(float x, float y)
{
	Body::moveEntity(x, y);
	m_enemyTarget_.x += x;
	m_enemyTarget_.y += y;
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

walkingVector Enemy::checkEnemyMove(World* p_world, float x, float y, float deltaTime)
{
	
	if (!x && !y)
		return { 0,0 };
	
	bool xCollision = false, yCollision = false;
	float xMovement = (x * deltaTime) * ENEMY_SPEED, yMovement = (y * deltaTime) * ENEMY_SPEED;

	//------------------------------------------------------------------------------------------- Detect collision on x-axis
	if (x != 0) {
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

	//------------------------------------------------------------------------------------------- Detect collision on y-axis
	if (y != 0) {
		this->moveEntity(0, yMovement);
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

		this->moveEntity(0, -yMovement);
	}

	//------------------------------------------------------------------------------------------- Detect collision on y-axis and x-axis
	if (!xCollision && !yCollision && x != 0 && y != 0) {
		bool bothCollisons = false;
		this->moveEntity(xMovement, yMovement);
		for (auto const& cursor : *p_world->getEnemyVector()) {
			if (SDL_HasIntersectionF(&m_bounds_, cursor->getBounds()) && this != cursor.get()) {
				bothCollisons = true;
				break;
			}
		}
		if (!bothCollisons) {
			for (auto const& cursor : *p_world->getEntityVector()) {
				if (SDL_HasIntersectionF(&m_bounds_, cursor->getBounds()) && this != cursor.get()) {
					bothCollisons = true;
					break;
				}
			}
		}
		this->moveEntity(-xMovement, -yMovement);

		if (bothCollisons) {		//The weaker movement should be paused in order to avoid moving inside an obstacle
			if (abs(x) > abs(y))
				yCollision = true;
			else
				xCollision = true;
		}

			
	}

	return { x * !xCollision, y * !yCollision };
}

Enemy::~Enemy()
{
	std::cout << "Enemy deleted" << std::endl;
}
