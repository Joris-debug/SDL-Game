#pragma once
#include "Body.h"
class PlayerTwo :
    public Body
{
private:
       SDL_Texture* m_p_textureIdle;
       SDL_Texture* m_p_textureRun;
       SDL_Texture* m_p_textureTurn;
       SDL_Texture* m_p_textureAttack;
       SDL_Texture* m_p_textureHit;
       bool m_currentDirection; // 0: right, 1 = left
public:
    PlayerTwo(SDL_Renderer* renderer, SDL_FPoint startPos);
    ~PlayerTwo();
    void renderBody(SDL_Renderer* renderer) override;
    void teleportPlayerTwo(SDL_FPoint newPos);
    void animateBody(float x, float y) override;
    SDL_FPoint* getPlayerTargets();     //Returns an array of 3 points, that resemble head, stomach and feet of the player (Enemies will target these points)
    inline void setCurrentDirection(bool m_currentDirection) { this->m_currentDirection = m_currentDirection; }
};

