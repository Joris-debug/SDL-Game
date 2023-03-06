#include "VirtualEnemy.h"

VirtualEnemy::VirtualEnemy(int m_enemyId, Uint8 m_enemyType, SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_)
	: Enemy(m_enemyType, m_p_textureIdle_, m_p_textureRun_, m_p_textureHit_, m_bounds_, m_spriteBounds_, m_maxLives_)
{

	delete m_p_lastDamageTaken;		//VirtualEnemy wont need these
	m_p_lastDamageTaken = nullptr;	
	delete m_p_lastTargetAssigned;
	m_p_lastTargetAssigned = nullptr;
	delete m_p_lastFrame;
	m_p_lastFrame = nullptr;
	delete m_p_timeSinceZeroMovement;
	m_p_timeSinceZeroMovement = nullptr;

	this->m_enemyId = m_enemyId;
}

void VirtualEnemy::enemyPathfinding(World* p_world, float deltaTime)
{
}

void VirtualEnemy::animateBody(float x, float y)
{
}

void VirtualEnemy::setAnimation(Uint8 mode, short frame)
{
	m_currentMode = static_cast<Mode>(mode);
	m_currentSprite = frame;
}

bool VirtualEnemy::damageBody(short damage)
{
	return false;
}


