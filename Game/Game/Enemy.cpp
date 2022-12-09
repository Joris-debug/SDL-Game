#include "Enemy.h"
#include "World.h"
#include "Player.h"
Enemy::Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_) : Body(m_bounds_, m_spriteBounds_, m_maxLives_)
{
	this->m_p_textureIdle_ = m_p_textureIdle_;
	this->m_p_textureRun_ = m_p_textureRun_;
	this->m_p_textureHit_ = m_p_textureHit_;
	m_TracedObstacle_ = ObstacleLocation::noObstacle;
	m_factor_ = (unsigned int)this % 5 + 3;	//Random number to set a certain speed (3-7)
	std::cout << m_factor_ << std::endl;
}

void Enemy::enemyPathfinding(World* p_world, float deltaTime)
{

	Player* p_enemyTarget = p_world->getPlayer()->get();
	SDL_FRect* targetLocation = p_enemyTarget->getBounds();

	if (isInvincible()) {	//Hurt Enemies don't move
		this->animateBody(0, 0);
		return;
	}

	short x = 0, y = 0, margin = 8;
	bool inPlayerRange = false;
	if (targetLocation->x + (targetLocation->w / 2.0) > m_bounds_.x + (m_bounds_.w / 2.0) + margin)
		x = 1;
	else if (targetLocation->x + (targetLocation->w / 2.0) + margin < m_bounds_.x + (m_bounds_.w / 2.0))
		x = -1;

	if (targetLocation->y + (targetLocation->h / 2.0) > m_bounds_.y + (m_bounds_.h / 2.0) + margin)
		y = 1;
	else if (targetLocation->y + (targetLocation->h / 2.0) + margin < m_bounds_.y + (m_bounds_.h / 2.0))
		y = -1;

	if (SDL_HasIntersectionF(targetLocation, &m_bounds_))
		inPlayerRange = true;

	if (m_TracedObstacle_ != ObstacleLocation::noObstacle) { //If the enemy detected an obstacle in its way
		switch (m_TracedObstacle_) {						 //This switch is needed to start evasive manoeuvres
		case ObstacleLocation::xObstacle_R:
			y = 1;
			break;
		case ObstacleLocation::xObstacle_L:
			y = -1;
			break;
		case ObstacleLocation::yObstacle_T:
			x = 1;
		case ObstacleLocation::yObstacle_B:
			x = -1;
			break;
		}
	}

	walkingVector legalMove = this->checkEnemyMove(p_world, x, y, deltaTime);
	walkingVector empty = { 0, 0 };
	if (!inPlayerRange && legalMove == empty && m_TracedObstacle_ == ObstacleLocation::noObstacle) {	//If the enemy stopped before reaching the player
		if (x == 1) 
			m_TracedObstacle_ = ObstacleLocation::xObstacle_R;
		else if(x == -1)
			m_TracedObstacle_ = ObstacleLocation::xObstacle_L;
		else if (y == 1)
			m_TracedObstacle_ = ObstacleLocation::yObstacle_B;
		else if (y == -1)
			m_TracedObstacle_ = ObstacleLocation::yObstacle_T;
	}

	this->moveBody((legalMove.x * deltaTime) * (m_factor_ / 10.0), (deltaTime * legalMove.y) * (m_factor_ / 10.0));
	this->animateBody(x, y);

	if (m_TracedObstacle_ != ObstacleLocation::noObstacle) {
		walkingVector xDirectionStruct = { x, 0 };
		walkingVector yDirectionStruct = { 0, y };
		switch (m_TracedObstacle_) {						//This switch is needed to see if the obstacle still needs to be traced 
		case ObstacleLocation::xObstacle_R:
		case ObstacleLocation::xObstacle_L:
			if (xDirectionStruct == this->checkEnemyMove(p_world, x, 0, 16) && legalMove.x) { //Only stop tracing an Object if there is enough room afterwards 
				m_TracedObstacle_ = ObstacleLocation::noObstacle;
			}
			break;

		case ObstacleLocation::yObstacle_T:
		case ObstacleLocation::yObstacle_B:
			if (yDirectionStruct == this->checkEnemyMove(p_world, 0, y, 16) && legalMove.y) {
				m_TracedObstacle_ = ObstacleLocation::noObstacle;
			}
			break;
		}
	}

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

walkingVector Enemy::checkEnemyMove(World* p_world, int x, int y, float deltaTime)
{
	bool xCollision = false, yCollision = false;
	float xMovement = (x * deltaTime) * (m_factor_ / 10.0), yMovement = (y * deltaTime) * (m_factor_ / 10.0);

	if (!x && !y)
		return { 0,0 };


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

		if(bothCollisons)
			switch (m_TracedObstacle_) {			//This switch is needed to start evasive manoeuvres
			case ObstacleLocation::xObstacle_R:
			case ObstacleLocation::xObstacle_L:
				yCollision = true;
				break;
			case ObstacleLocation::yObstacle_B:
			case ObstacleLocation::yObstacle_T:
				xCollision = true;
				break;
			default:
				xCollision = true;
				break;
			}
	}

	return { x * !xCollision, y * !yCollision };
}

Enemy::~Enemy()
{
	std::cout << "Enemy deleted" << std::endl;
}
