#pragma once
#include "Body.h"

enum class ObstacleLocation {noObstacle, xObstacle, yObstacle};

class World;
class Enemy :
    public Body
{
protected:
    SDL_Texture* m_p_textureIdle_;
    SDL_Texture* m_p_textureRun_;
    SDL_Texture* m_p_textureHit_;
    ObstacleLocation m_TracedObstacle_;
    short m_factor_;    //A set factor for calculating speed and other actions
public:
    Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_Texture* m_p_textureHit_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_, short m_maxLives_);
    void enemyPathfinding(World* p_world, float deltaTime);
    void animateBody(int x, int y) override;
    void renderBody(SDL_Renderer* renderer, double pixel_per_pixel) override;
    walkingVector checkEnemyMove(World* p_world, int x, int y, float deltaTime);        //Returns a legal move
    ~Enemy();
};

