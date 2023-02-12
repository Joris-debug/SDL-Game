#include "Enemy.h"
#include "World.h"
#include "Player.h"
#include "Resources.h"
Enemy::Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_) : Body(m_bounds_, m_spriteBounds_, m_maxLives_)
{
	this->m_p_textureIdle_ = m_p_textureIdle_;
	this->m_p_textureRun_ = m_p_textureRun_;
	this->m_p_textureHit_ = m_p_textureHit_;
	m_factor_ = (Uint32)this % 5 + 3;	//Random number for certain calculations
	m_p_lastTargetAssigned_ = new Clock(1000);
	m_p_timeSinceZeroMovement_ = new Clock(600);
	m_p_lastTargetAssigned_->setStartPoint(0);
	m_enemyTarget_ = { m_bounds_.x + m_bounds_.w / 2 , m_bounds_.y + m_bounds_.h / 2 }; //His first target is himself, on the next run of the enemyPathfinding method, a new target should be assigned
}

void Enemy::enemyPathfinding(World* p_world, float deltaTime)
{
	if (isInvincible()) {	//Hurt Enemies don't move
		this->animateBody(0, 0);
		return;
	}
	

	SDL_FPoint enemyMiddle = { m_bounds.x + m_bounds.w / 2, m_bounds.y + m_bounds.h / 2 };

	if (m_p_lastTargetAssigned_->checkClockState()) {
		//----------------------------------------------------------------------------------------------- Checking if the player has been spotted
		SDL_FPoint* p_playerTargets = p_world->getPlayer()->getPlayerTargets();
		bool playerSpotted = true;
		auto p_entityVector = p_world->getEntityVector();
		for (short targetNr = 0; targetNr < 3; targetNr++) {				//Looping the three possible target options			
			auto it = p_entityVector->begin();
			while (it != p_entityVector->end()) {					//Looping for every entity on the map
				if (SDL_IntersectFRectAndLine((*it)->getBounds(), &p_playerTargets[targetNr].x, &p_playerTargets[targetNr].y, &enemyMiddle.x, &enemyMiddle.y)) {
					playerSpotted = false;
					break;
				}
				it++;
			}
			if (playerSpotted) {
				m_enemyTarget_ = p_playerTargets[targetNr];
				break;
			}
			else if (targetNr < 3) {		//Im reseting the variables for the next run
				playerSpotted = true;
				enemyMiddle = { m_bounds.x + m_bounds.w / 2, m_bounds.y + m_bounds.h / 2 };
			}
		}

		short margin = 8;
		if (abs(enemyMiddle.x - m_enemyTarget_.x) < margin && abs(enemyMiddle.y - m_enemyTarget_.y) < margin) {		//New direction assigned if the old target is reached
			m_enemyTarget_ = p_world->getRandomCoordinate();
		}
		else if (p_world->getRandomNumber(0, 200) == Uint32(this) % 10 && !playerSpotted) {			//New direction assigned if a random check is hit
			m_enemyTarget_ = p_world->getRandomCoordinate();
		}
	}

	float dirX = m_enemyTarget_.x - enemyMiddle.x;
	float dirY = m_enemyTarget_.y - enemyMiddle.y;

	if (abs(dirX) < 8) 
		dirX = 0;
	if (abs(dirY) < 8)
		dirY = 0;

	float hyp = (dirX || dirY) ? sqrt(dirX * dirX + dirY * dirY) : 1;
	dirX /= hyp;
	dirY /= hyp;
	
	walkingVector legalMove = this->checkEnemyMove(p_world, dirX, dirY, deltaTime);
	if (abs(legalMove.x) < 0.1 && abs(legalMove.y) < 0.1 && m_p_timeSinceZeroMovement_->checkClockState()) {	//If the enemy stopped before reaching the target
		//std::cout << "0 movement\n";
		m_enemyTarget_ = p_world->getRandomCoordinate();
		m_p_lastTargetAssigned_->setStartPoint(SDL_GetTicks());
	}
	Body::moveEntity((legalMove.x * deltaTime) * ENEMY_SPEED, (deltaTime * legalMove.y) * ENEMY_SPEED); //This method does not move the enemy target
	animateBody(dirX, dirY);
}

void Enemy::animateBody(float x, float y)
{
	int totalSprites = 1;
	int spriteLayer = 0;

 	do {
		if (isInvincible()) {
			m_currentMode = Mode::hit;
			totalSprites = 6;
			break;
		}

		if (!x && !y) {
			m_currentMode = Mode::idle;
			totalSprites = 4;
			break;
		}

		if (abs(x) - abs(y) < 0.1f) {
			x = 0.0f;
		}

		if (abs(x) > abs(y)) {

			if (x > 0) {
				m_currentMode = Mode::walk;
				totalSprites = 4;
				spriteLayer = 1;
				break;
			}

			if (x < 0) {
				m_currentMode = Mode::walk;
				totalSprites = 4;
				spriteLayer = 2;
				break;
			}
		}
		else {

			if (y > 0) {
				m_currentMode = Mode::walk;
				totalSprites = 4;
				spriteLayer = 0;
				break;
			}

			if (y < 0) {
				m_currentMode = Mode::walk;
				totalSprites = 4;
				spriteLayer = 3;
				break;
			}
		}

	} while (false);

	if (m_p_lastFrame->checkClockState()) {	//Next sprite
		m_currentSprite++;
	}

	if (m_currentSprite >= totalSprites) {		//End of spritesheet
		m_currentSprite = 0;
	}

	m_textureCoords.x = m_textureCoords.w * m_currentSprite;
	m_textureCoords.y = m_textureCoords.h * spriteLayer;

}

void Enemy::moveEntity(float x, float y)
{
	Body::moveEntity(x, y);
	m_enemyTarget_.x += x;
	m_enemyTarget_.y += y;
}

void Enemy::renderBody(SDL_Renderer* renderer)
{
	switch (m_currentMode) {
	case Mode::idle:
		SDL_RenderCopyF(renderer, m_p_textureIdle_, &m_textureCoords, &m_spriteBounds);
		break;
	case Mode::walk:
		SDL_RenderCopyF(renderer, m_p_textureRun_, &m_textureCoords, &m_spriteBounds);
		break;
	case Mode::hit:
		SDL_RenderCopyF(renderer, m_p_textureHit_, &m_textureCoords, &m_spriteBounds);
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
			if (SDL_HasIntersectionF(&m_bounds, cursor->getBounds()) && this != cursor) {
				xCollision = true;
				break;
			}
		}

		if (!xCollision) {
			for (auto const& cursor : *p_world->getEntityVector()) {
				if (SDL_HasIntersectionF(&m_bounds, cursor->getBounds()) && this != cursor) {
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
			if (SDL_HasIntersectionF(&m_bounds, cursor->getBounds()) && this != cursor) {
				yCollision = true;
				break;
			}
		}

		if (!yCollision) {
			for (auto const& cursor : *p_world->getEntityVector()) {
				if (SDL_HasIntersectionF(&m_bounds, cursor->getBounds()) && this != cursor) {
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
			if (SDL_HasIntersectionF(&m_bounds, cursor->getBounds()) && this != cursor) {
				bothCollisons = true;
				break;
			}
		}
		if (!bothCollisons) {
			for (auto const& cursor : *p_world->getEntityVector()) {
				if (SDL_HasIntersectionF(&m_bounds, cursor->getBounds()) && this != cursor) {
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
	delete m_p_lastTargetAssigned_;
	delete m_p_timeSinceZeroMovement_;
	//std::cout << "Enemy deleted" << std::endl;
}
