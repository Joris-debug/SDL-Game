#pragma once
#include "Body.h"
class Enemy :
    public Body
{
protected:
    SDL_Texture* m_p_textureIdle_;
    SDL_Texture* m_p_textureRun_;
public:
    Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_);
    walkingVector enemyPathfinding();
    void animateBody(int x, int y);
    void renderBody(SDL_Renderer* renderer, double pixel_per_pixel);
    ~Enemy();
};

