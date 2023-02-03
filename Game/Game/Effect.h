#pragma once
#include "Entity.h"
#include "Clock.h"
class Effect :
    public Entity
{
private:
    bool m_effectIsDone_;
    Clock* m_p_lastFrame_;		// Timestamp of the last frame
    short m_currentSprite_;		// The current sprite of the animation that is playing
    SDL_Rect m_textureCoords_;
    SDL_Texture* m_p_effectTextures_;   //Stores the animation of the effect
public:
    Effect(SDL_FRect m_bounds_, SDL_Texture* m_p_effectTextures_);
    ~Effect();
    void animateEffect();
    void renderEffect(SDL_Renderer* renderer);
    inline void setEffectIsDone(bool m_effectIsDone_) { this->m_effectIsDone_ = m_effectIsDone_; }
    inline bool getEffectIsDone() { return m_effectIsDone_; }
    inline short getCurrentSprite() { return (m_effectIsDone_) ? 10 : m_currentSprite_; }
};

