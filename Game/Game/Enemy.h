#pragma once
#include "Body.h"

class World;
class Enemy :
    public Body
{
protected:
    static int m_s_enemyCount;
    Clock* m_p_lastTargetAssigned;
    Clock* m_p_timeSinceZeroMovement;
    SDL_Texture* m_p_textureIdle;
    SDL_Texture* m_p_textureRun;
    SDL_Texture* m_p_textureHit;
    SDL_FPoint m_enemyTarget;
    int m_enemyId;    
public:
    Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_);
    virtual void enemyPathfinding(World* p_world, float deltaTime);
    void animateBody(float x, float y) override;
    void moveEntity(float x, float y) override;
    void renderBody(SDL_Renderer* renderer);
    virtual walkingVector checkEnemyMove(World* p_world, float x, float y, float deltaTime);        //Returns a legal move
    ~Enemy();
};

