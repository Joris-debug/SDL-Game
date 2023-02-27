#pragma once
#include "Enemy.h"

enum class enemyType{mantis, maggot, beetle};

class VirtualEnemy :
    public Enemy
{
private:
    enemyType m_enemyType;
public:
    VirtualEnemy(enemyType m_enemyType, SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_);
    void enemyPathfinding(World* p_world, float deltaTime) override;
    void animateBody(float x, float y) override;
    void setAnimation(Uint8 mode, short frame);
};

