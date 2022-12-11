#pragma once
#include <iostream>
#include "SDL.h"
#include "Body.h"
class Enemy;
class Player :
    public Body
{
private:
    SDL_FRect m_footSpace_; 
    SDL_Texture* m_p_textureIdle_;
    SDL_Texture* m_p_textureRun_;
    SDL_Texture* m_p_textureTurn_;
    SDL_Texture* m_p_textureAttack_;
    bool m_isAttacking_;
    bool m_isTurning_;
public:
    Player(SDL_Renderer *renderer);
    ~Player();
    bool detectTurning(int x, int y);
    void animateBody(float x, float y) override;
    void renderBody(SDL_Renderer *renderer, double pixel_per_pixel) override;
    inline bool getIsAttacking() { return m_isAttacking_; }
	inline void setIsAttacking() { m_isAttacking_ = true; m_currentSprite_ = 0; }    
    inline void moveFootSpace(float x, float y) { m_footSpace_.x += x; m_footSpace_.y += y; }
	inline SDL_FRect* getFootSpace() { return &m_footSpace_; }
    SDL_FPoint* getPlayerTargets(); //Returns an array of 3 points, that resemble head, stomach and feet of the player (Enemies will target these points)
};

