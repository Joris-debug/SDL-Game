#include "Beetle.h"
#include "World.h"
#include "Player.h"
#include "Resources.h"

void Beetle::enemyPathfinding(World* p_world, float deltaTime)
{
	if (isInvincible()) {	//Hurt Enemies don't move
		this->animateBody(0, 0);
		return;
	}
	
	SDL_FPoint enemyMiddle = { m_bounds.x + m_bounds.w / 2, m_bounds.y + m_bounds.h / 2 };

	if (m_p_lastTargetAssigned->checkClockState()) {
		//----------------------------------------------------------------------------------------------- Checking if the player has been spotted
		SDL_FPoint* p_playerTargets = p_world->getPlayer()->getPlayerTargets();
		m_enemyTarget = p_playerTargets[0];

		short margin = 8;
		if (abs(enemyMiddle.x - m_enemyTarget.x) < margin && abs(enemyMiddle.y - m_enemyTarget.y) < margin) {		//New direction assigned if the old target is reached
			m_enemyTarget = p_world->getRandomCoordinate();
		}

	}

	float dirX = m_enemyTarget.x - enemyMiddle.x;
	float dirY = m_enemyTarget.y - enemyMiddle.y;

	if (abs(dirX) < 8)
		dirX = 0;
	if (abs(dirY) < 8)
		dirY = 0;

	float hyp = (dirX || dirY) ? sqrt(dirX * dirX + dirY * dirY) : 1;
	dirX /= hyp;
	dirY /= hyp;

	walkingVector legalMove = this->checkEnemyMove(p_world, dirX, dirY, deltaTime);
	if (abs(legalMove.x) < 0.1 && abs(legalMove.y) < 0.1 && m_p_timeSinceZeroMovement->checkClockState()) {	//If the enemy stopped before reaching the target
		//std::cout << "0 movement\n";
		m_enemyTarget = p_world->getRandomCoordinate();
		m_p_lastTargetAssigned->setStartPoint(SDL_GetTicks());
	}
	Body::moveEntity((legalMove.x * deltaTime) * ENEMY_SPEED * 1.5f, (deltaTime * legalMove.y) * ENEMY_SPEED * 1.5f); //This method does not move the enemy target
	animateBody(dirX, dirY);
}

Beetle::Beetle(Uint8 m_enemyType, SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_)
	: Enemy(m_enemyType, m_p_textureIdle_, m_p_textureRun_, m_p_textureHit_, m_bounds_, m_spriteBounds_, m_maxLives_)
{
}

walkingVector Beetle::checkEnemyMove(World* p_world, float x, float y, float deltaTime)
{
	if (!x && !y)
		return { 0,0 };

	bool xCollision = false, yCollision = false;
	float xMovement = (x * deltaTime) * ENEMY_SPEED * 1.5f, yMovement = (y * deltaTime) * ENEMY_SPEED * 1.5f;
	
	//------------------------------------------------------------------------------------------- Detect collision on x-axis
	if (x != 0) {
		this->moveEntity(xMovement, 0);

		for (auto cursor : *p_world->getEnemyVector()) {
			if (typeid(*cursor) == typeid(Beetle) && SDL_HasIntersectionF(&m_bounds, cursor->getBounds()) && this != cursor) {
				xCollision = true;
				break;
			}
		}

		this->moveEntity(-xMovement, 0);
	}

	//------------------------------------------------------------------------------------------- Detect collision on y-axis
	if (y != 0) {
		this->moveEntity(0, yMovement);
		for (auto cursor : *p_world->getEnemyVector()) {
			if (typeid(*cursor) == typeid(Beetle) && SDL_HasIntersectionF(&m_bounds, cursor->getBounds()) && this != cursor) {
				yCollision = true;
				break;
			}
		}

		this->moveEntity(0, -yMovement);
	}

	//------------------------------------------------------------------------------------------- Detect collision on y-axis and x-axis
	if (!xCollision && !yCollision && x != 0 && y != 0) {
		bool bothCollisons = false;
		this->moveEntity(xMovement, yMovement);
		for (auto cursor : *p_world->getEnemyVector()) {
			if (typeid(*cursor) == typeid(Beetle) && SDL_HasIntersectionF(&m_bounds, cursor->getBounds()) && this != cursor) {
				bothCollisons = true;
				break;
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

