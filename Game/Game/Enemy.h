#pragma once
#include "Body.h"


class World;
class Enemy :
    public Body
{
protected:
    Uint32 m_lastTargetAssigned_;
    SDL_Texture* m_p_textureIdle_;
    SDL_Texture* m_p_textureRun_;
    SDL_Texture* m_p_textureHit_;
    SDL_FPoint m_enemyTarget_;
    short m_factor_;    //A set factor for calculating speed and other actions
public:
    Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_);
    void enemyPathfinding(World* p_world, float deltaTime);
    void animateBody(float x, float y) override;
    void moveEntity(float x, float y) override;
    void renderBody(SDL_Renderer* renderer) override;
    walkingVector checkEnemyMove(World* p_world, float x, float y, float deltaTime);        //Returns a legal move
    ~Enemy();
};

