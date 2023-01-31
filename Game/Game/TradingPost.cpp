#include "TradingPost.h"
#include "Resources.h"
#include "SDL_image.h"

TradingPost::TradingPost(SDL_Renderer* renderer) : Entity({ 100, 200, 56 * 2 , 56 * 2 })
{
    SDL_Surface* tmpSurface = IMG_Load(RSC_TRADING_POST_BACKGROUND);
    m_TradingPostTextures_[0] = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);
    
    tmpSurface = IMG_Load(RSC_TRADING_POST_FOREGROUND);
    m_TradingPostTextures_[1] = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_IDLE);
    m_merchantTextures_.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_GREETING);
    m_merchantTextures_.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_BORED1);
    m_merchantTextures_.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_BORED2);
    m_merchantTextures_.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_WORK1);
    m_merchantTextures_.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_WORK2);
    m_merchantTextures_.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    memset(m_upgrade1Sold_, 0, 2);
    memset(m_upgrade2Sold_, 0, 2);
    memset(m_upgrade3Sold_, 0, 2);

    m_p_lastFrame_ = new Clock(167);
    m_currentMode_ = MerchantMode::idle;
    m_currentSprite_ = 0;
    m_textureCoords_ = { 0, 0, 32, 32 };
    animateMerchant();

}

TradingPost::~TradingPost()
{
    int numberOfElements = m_merchantTextures_.size();
    for (int i = 0; i < numberOfElements; i++) {
        SDL_DestroyTexture(m_merchantTextures_.back());
        m_merchantTextures_.pop_back();
    }

    SDL_DestroyTexture(m_TradingPostTextures_[0]);
    SDL_DestroyTexture(m_TradingPostTextures_[1]);
}

void TradingPost::animateMerchant()
{
    m_textureCoords_ = { 0, 0, 32, 32 };
    int totalSprites = 4;

    switch (m_currentMode_)
    {
    case MerchantMode::idle:
    case MerchantMode::greeting:
        totalSprites = 4;
        break;
    case MerchantMode::bored1:
    case MerchantMode::bored2:
        totalSprites = 10;
        break;
    case MerchantMode::work1:
    case MerchantMode::work2:
        totalSprites = 5;
        break;
    }


    if (m_p_lastFrame_->checkClockState()) {	//Next sprite
        m_currentSprite_++;
    }

    if (m_currentSprite_ >= totalSprites) {		//End of spritesheet
        m_currentSprite_ = 0;
        if (m_currentMode_ == MerchantMode::work2) {
            m_currentMode_ = MerchantMode::idle;
        }
        else
            m_currentMode_ = MerchantMode(int(m_currentMode_) + 1);
    }

    m_textureCoords_.x = m_textureCoords_.w * m_currentSprite_;
}

void TradingPost::renderTradingPost(SDL_Renderer* renderer)
{
    animateMerchant();
    //The merchant is standing a bit offset
    SDL_FRect merchantRect = m_bounds_;
    merchantRect.x += 26;
    merchantRect.y += 40;
    merchantRect.w = 64;
    merchantRect.h = 64;


    SDL_RenderCopyF(renderer, m_TradingPostTextures_[0], NULL, &m_bounds_);
    SDL_RenderCopyF(renderer, m_merchantTextures_[int(m_currentMode_)], &m_textureCoords_, &merchantRect);
    SDL_RenderCopyF(renderer, m_TradingPostTextures_[1], NULL, &m_bounds_);
}
