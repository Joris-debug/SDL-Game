#pragma once
#include "Enemy.h"
class Beetle :
	public Enemy
{
public:
	void enemyPathfinding(World* p_world, float deltaTime) override;
	Beetle(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_);
	walkingVector checkEnemyMove(World* p_world, float x, float y, float deltaTime) override;        //Returns a legal move
};

