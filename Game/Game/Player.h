#pragma once
#include "SDL.h"
#include <iostream>
#include "Body.h"
#include <list>
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
    short m_playerLives_;
public:
    Player(SDL_Renderer *renderer);
    ~Player();
    bool detectTurning(int x, int y);
    void animateBody(int x, int y) override;
    void attack(std::list <std::unique_ptr<Enemy>>* entityList);
    void renderBody(SDL_Renderer *renderer, double pixel_per_pixel) override;
    bool inline getIsAttacking() { return m_isAttacking_; }
    inline SDL_FRect* getFootSpace() { return &m_footSpace_; }
    inline void moveFootSpace(float x, float y) { m_footSpace_.x += x; m_footSpace_.y += y; }
};

