#pragma once
#include "SDL.h"
class Player
{
private:    
    SDL_Texture* m_p_textureIdle_;
    SDL_FRect m_bounds_;
    SDL_FRect m_spriteBounds_;
    SDL_Rect m_spriteCoords_;
    int currentMode; // 1: Idle, 2:
public:
    Player(SDL_Renderer *renderer);
    ~Player();
    void animatePlayer();
    void renderPlayer(SDL_Renderer *renderer, double pixel_per_pixel);
};

