#pragma once
#include "Entity.h"
#include <vector>
#include "Clock.h"
#include <random>

enum class MerchantMode { idle, greeting, bored1, bored2, work1, work2 };

class Effect;

class TradingPost :
    public Entity
{
private:
    bool m_isActive;   //stores if the merchant is visible or not
    Effect* m_p_spawnEffect;
    std::mt19937* m_p_randomNumberEngine;
    Clock* m_p_lastFrame;		// Timestamp of the last frame
    short m_currentSprite;		// The current sprite of the animation that is playing
    MerchantMode m_currentMode;
    SDL_Rect m_textureCoords;    
    SDL_Texture* m_p_TradingPostTextures[3]; //Stores the back- and foreground of the building as well as the roof
    std::vector<SDL_Texture*> m_merchantTextures;  //Stores every animation of the merchant
    int m_upgrade1Sold[2];    //How many times upgrade 1 Sold | [0] = player 1, [1] = player 2
    int m_upgrade2Sold[2];
    int m_upgrade3Sold[2];
public:
    TradingPost(SDL_Renderer* renderer, std::mt19937* m_p_randomNumberEngine_, Effect* m_p_spawnEffect_);
    ~TradingPost();
    inline Effect* getSpawnEffect() { return m_p_spawnEffect; }
    inline void setIsActive(bool m_isActive_) { this->m_isActive = m_isActive_; }
    inline bool getIsActive() { return m_isActive; }
    void positionExplosion();
    void randomizeMode();
    void animateMerchant();
    void renderTradingPost(SDL_Renderer* renderer);
    void renderTradingPostRoof(SDL_Renderer* renderer);
};

