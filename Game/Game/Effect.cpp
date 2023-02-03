#include "Effect.h"

Effect::Effect(SDL_FRect m_bounds_, SDL_Texture* m_p_effectTextures_) : Entity(m_bounds_)
{
    m_effectIsDone_ = false;
    m_p_lastFrame_ = new Clock(100);
    m_currentSprite_ = 0;
    m_textureCoords_ = { 0, 0, int(m_bounds_.w) / 2, int(m_bounds_.h) / 2 };
    this->m_p_effectTextures_ = m_p_effectTextures_;

}

Effect::~Effect()
{
    delete m_p_lastFrame_;
}

void Effect::animateEffect()
{
    if (!m_effectIsDone_ && m_p_lastFrame_->checkClockState()) {	//Next sprite
        m_currentSprite_++;
    }

    if (m_currentSprite_ >= 10) {		//End of spritesheet
        m_currentSprite_ = 0;
        m_effectIsDone_ = true;
    }

    m_textureCoords_.x = m_textureCoords_.w * m_currentSprite_;
}

void Effect::renderEffect(SDL_Renderer* renderer)
{
    if (m_effectIsDone_)
        return;

    SDL_RenderCopyF(renderer, m_p_effectTextures_, &m_textureCoords_, &m_bounds_);
}
