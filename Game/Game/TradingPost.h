#pragma once
#include "Entity.h"
#include <vector>
#include "Clock.h"
#include <random>

enum class MerchantMode { idle, greeting, bored1, bored2, work1, work2 };


class TradingPost :
    public Entity
{
private:
    std::mt19937* m_p_randomNumberEngine_;
    Clock* m_p_lastFrame_;		// Timestamp of the last frame
    short m_currentSprite_;		// The current sprite of the animation that is playing
    MerchantMode m_currentMode_;
    SDL_Rect m_textureCoords_;    
    SDL_Texture* m_TradingPostTextures_[3]; //Stores the back- and foreground of the building as well as the roof
    std::vector<SDL_Texture*> m_merchantTextures_;  //Stores every animation of the merchant
    int m_upgrade1Sold_[2];    //How many times upgrade 1 Sold | [0] = player 1, [1] = player 2
    int m_upgrade2Sold_[2];
    int m_upgrade3Sold_[2];
public:
    TradingPost(SDL_Renderer* renderer, std::mt19937* m_p_randomNumberEngine_);
    ~TradingPost();
    void randomizeMode();
    void animateMerchant();
    void renderTradingPost(SDL_Renderer* renderer);
    void renderTradingPostRoof(SDL_Renderer* renderer);
};

