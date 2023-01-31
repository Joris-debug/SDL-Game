#pragma once
#include "Entity.h"
#include <vector>
#include "Clock.h"

enum class MerchantMode { idle, greeting, bored1, bored2, work1, work2 };


class TradingPost :
    public Entity
{
private:
    Clock* m_p_lastFrame_;		// Timestamp of the last frame
    MerchantMode m_currentMode_;
    short m_currentSprite_;		// The current sprite of the animation that is playing
    SDL_Rect m_textureCoords_;
    SDL_Rect m_triggerRadius_;  //In this area, the player is able to talk to the merchant
    SDL_Texture* m_TradingPostTextures_[2]; //Stores the back- and foreground of the building
    std::vector<SDL_Texture*> m_merchantTextures_;  //Stores every animation of the merchant
    int m_upgrade1Sold_[2];    //How many times upgrade 1 Sold | [0] = player 1, [1] = player 2
    int m_upgrade2Sold_[2];
    int m_upgrade3Sold_[2];
public:
    TradingPost(SDL_Renderer* renderer);
    ~TradingPost();
    void animateMerchant();
    void renderTradingPost(SDL_Renderer* renderer);
};

