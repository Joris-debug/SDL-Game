#pragma once
#include "SDL.h"
#include "Body.h"
#include <list>
class Player :
    public Body
{
private:    
    SDL_Texture* m_p_textureIdle_;
    SDL_Texture* m_p_textureRun_;
    SDL_Texture* m_p_textureTurn_;
    SDL_FRect m_footSpace_;
    bool m_isTurning_; 
public:
    Player(SDL_Renderer *renderer);
    ~Player();
    bool detectTurning(int x, int y);
    void animateBody(int x, int y) override;
    void attack(std::list <Entity*> entityList);
    void renderBody(SDL_Renderer *renderer, double pixel_per_pixel) override;
};

