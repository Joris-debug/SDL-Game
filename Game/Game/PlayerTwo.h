#pragma once
#include "Body.h"
class PlayerTwo :
    public Body
{
private:
       SDL_Texture* m_p_textureIdle;
       SDL_Texture* m_p_textureRun;
       SDL_Texture* m_p_textureTurn;
       SDL_Texture* m_p_textureAttack;
       SDL_Texture* m_p_textureHit;
};

