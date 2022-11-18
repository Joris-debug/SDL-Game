#pragma once
#include "SDL.h"
#include "Body.h"
class Player :
    public Body
{
private:    
    SDL_Texture* m_p_textureIdle_;
    SDL_Texture* m_p_textureRun_;
    SDL_Texture* m_p_textureTurn_;
    bool m_isTurning_; 
public:
    Player(SDL_Renderer *renderer);
    ~Player();
    bool detectTurning(int x, int y);
    void animatePlayer(int x, int y);
    void renderPlayer(SDL_Renderer *renderer, double pixel_per_pixel);
};

