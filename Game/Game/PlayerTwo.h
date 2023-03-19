#pragma once
#include "Body.h"
#include "SoundHandler.h"
class PlayerTwo :
    public Body
{
private:
       SDL_Texture* m_p_textureIdle;
       SDL_Texture* m_p_textureRun;
       SDL_Texture* m_p_textureTurn;
       SDL_Texture* m_p_textureAttack;
       SDL_Texture* m_p_textureHit;
       SDL_FRect m_footSpace;
       bool m_currentDirection; // 0: right, 1 = left
       bool m_playerType;   //0 = server, 1 = client
       bool m_hitDetected;
       Clock* m_p_lastAttackTrigger;       // Timestamp of the last attack trigger
public:
    PlayerTwo(bool m_playerType, SDL_Renderer* renderer, SDL_FPoint startPos);
    ~PlayerTwo();
    void renderBody(SDL_Renderer* renderer) override;
    void teleportPlayerTwo(SDL_FPoint newPos);
    void animateBody(float x, float y) override;
    bool damageBody(short damage) override;
    void moveEntity(float x, float y) override;
    SDL_FRect* getPlayerTwoScreen();
    inline SDL_FRect* getFootSpace() { return &m_footSpace; }
    inline void moveFootSpace(float x, float y) { m_footSpace.x += x; m_footSpace.y += y; }
    inline void triggerNewAttack() { m_p_lastAttackTrigger->setStartPoint(SDL_GetTicks()); SoundHandler::getInstance().playSwordSound(); }
    inline bool isAttacking() { return !m_p_lastAttackTrigger->checkClockState(); }
    inline bool getHitDetected() { return (m_hitDetected) ? !(m_hitDetected = false) : false; }
    inline void setHitDetected(bool m_hitDetected) { this->m_hitDetected = m_hitDetected; }
    SDL_FPoint* getPlayerTwoTargets();     //Returns an array of 3 points, that resemble head, stomach and feet of the player (Enemies will target these points)
    inline bool getPlayerType() { return m_playerType; }        //False: PlayerTwo is the server, true: PlayerTwo is the client
    inline void setCurrentDirection(bool m_currentDirection) { this->m_currentDirection = m_currentDirection; }

};

