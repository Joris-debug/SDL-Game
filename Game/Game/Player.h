#pragma once
#include <iostream>
#include "SDL.h"
#include "Body.h"
class Enemy;
class Player :
    public Body
{
private:
    SDL_FRect m_footSpace; 
    SDL_Texture* m_p_textureIdle;
    SDL_Texture* m_p_textureRun;
    SDL_Texture* m_p_textureTurn;
    SDL_Texture* m_p_textureAttack;
    SDL_Texture* m_p_textureHit;
    Clock* m_p_lastAttack;       // Timestamp of the last attack
    int m_coinCounter;
    bool m_currentDirection; // 0: right, 1 = left
    bool m_lastDirection; // 0: right, 1 = left
	int m_attackCooldown;
public:
    Player(SDL_Renderer *renderer);
    ~Player();
    bool detectTurning();
    void animateBody(float x, float y) override;
    void renderBody(SDL_Renderer *renderer) override;
    float getAttackCooldownPercent();
    SDL_FPoint* getPlayerTargets();     //Returns an array of 3 points, that resemble head, stomach and feet of the player (Enemies will target these points)
    inline void updateAttackCooldown(int sum) { m_attackCooldown += sum; m_p_lastAttack->setInterval(m_attackCooldown); };
    inline int getCoinCounter() { return m_coinCounter; }
    inline void updateCoinCounter(int sum) { m_coinCounter += sum; }
    inline bool checkAttackCooldown() { return m_p_lastAttack->checkClockState(); }
    inline bool getIsAttacking() { return (m_currentMode == Mode::attack); }
    inline void setIsAttacking() { m_currentMode = Mode::attack; m_currentSprite = 0; m_p_lastAttack->setStartPoint(SDL_GetTicks()); }
    inline void moveFootSpace(float x, float y) { m_footSpace.x += x; m_footSpace.y += y; }
	inline SDL_FRect* getFootSpace() { return &m_footSpace; }
};

