#include "TradingPost.h"
#include "Resources.h"
#include "Effect.h"
#include "SDL_image.h"

TradingPost::TradingPost(SDL_Renderer* renderer, std::mt19937* m_p_randomNumberEngine_, Effect* m_p_spawnEffect_) : Entity({ 100, 200, 56 * 2 , 41 * 2 })
{
    this->m_p_randomNumberEngine = m_p_randomNumberEngine_;
    this-> m_p_spawnEffect = m_p_spawnEffect_;
    SDL_Surface* tmpSurface = IMG_Load(RSC_TRADING_POST_BACKGROUND);
    m_p_TradingPostTextures[0] = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);
    
    tmpSurface = IMG_Load(RSC_TRADING_POST_FOREGROUND);
    m_p_TradingPostTextures[1] = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_TRADING_POST_ROOF);
    m_p_TradingPostTextures[2] = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_IDLE);
    m_merchantTextures.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_GREETING);
    m_merchantTextures.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_BORED1);
    m_merchantTextures.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_BORED2);
    m_merchantTextures.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_WORK1);
    m_merchantTextures.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

    tmpSurface = IMG_Load(RSC_MERCHANT_WORK2);
    m_merchantTextures.push_back(SDL_CreateTextureFromSurface(renderer, tmpSurface));
    SDL_FreeSurface(tmpSurface);

	m_upgrade1Sold = 0;
	m_upgrade2Sold = 0; 
	m_upgrade3Sold = 0;

    m_isActive = false;
    m_p_lastFrame = new Clock(167);
    m_currentMode = MerchantMode::idle;
    m_currentSprite = 0;
    m_textureCoords = { 0, 0, 32, 32 };
    animateMerchant();

}

TradingPost::~TradingPost()
{
    int numberOfElements = int(m_merchantTextures.size());
    for (int i = 0; i < numberOfElements; i++) {
        SDL_DestroyTexture(m_merchantTextures.back());
        m_merchantTextures.pop_back();
    }

    SDL_DestroyTexture(m_p_TradingPostTextures[0]);
    SDL_DestroyTexture(m_p_TradingPostTextures[1]);
}

void TradingPost::positionExplosion()
{
    SDL_FRect* p_explosionBounds = m_p_spawnEffect->getBounds();
    p_explosionBounds->x = m_bounds.x - 36 * 2;
    p_explosionBounds->y = m_bounds.y - 39 * 2;
}

void TradingPost::randomizeMode()
{
    std::uniform_int_distribution<int> distribution(-15, 5);
    int tmpMode = distribution(*m_p_randomNumberEngine);
    if (tmpMode <= 0) {
        m_currentMode = MerchantMode::idle;
        return;
    }
    m_currentMode = MerchantMode(tmpMode);
}

void TradingPost::animateMerchant()
{
    m_textureCoords = { 0, 0, 32, 32 };
    int totalSprites = 4;

    switch (m_currentMode)
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


    if (m_p_lastFrame->checkClockState()) {	//Next sprite
        m_currentSprite++;
    }

    if (m_currentSprite >= totalSprites) {		//End of spritesheet
        m_currentSprite = 0;
        randomizeMode();
    }

    m_textureCoords.x = m_textureCoords.w * m_currentSprite;
}

void TradingPost::renderTradingPost(SDL_Renderer* renderer)
{

    m_p_spawnEffect->animateEffect(); // The current Frame needs to be up to date for the next check

    if (m_isActive && m_p_spawnEffect->getCurrentSprite() < 7)   //Merchant is not visible yet
        return;
    if (!m_isActive && m_p_spawnEffect->getCurrentSprite() > 5)
        return;


    animateMerchant();
    //The merchant is standing a bit offset
    SDL_FRect merchantRect = m_bounds;
    merchantRect.x += 26;
    merchantRect.y += 10;
    merchantRect.w = 64;
    merchantRect.h = 64;


    SDL_RenderCopyF(renderer, m_p_TradingPostTextures[0], NULL, &m_bounds);
    SDL_RenderCopyF(renderer, m_merchantTextures[int(m_currentMode)], &m_textureCoords, &merchantRect);
    SDL_RenderCopyF(renderer, m_p_TradingPostTextures[1], NULL, &m_bounds);
}

void TradingPost::renderTradingPostRoof(SDL_Renderer* renderer)
{
    bool roofVisible = true;
    if (m_isActive && m_p_spawnEffect->getCurrentSprite() < 7)   //Merchant is not visible yet
        roofVisible = false;
    if (!m_isActive && m_p_spawnEffect->getCurrentSprite() >= 7)
        roofVisible = false;

    if (roofVisible) {  //Merchant is not visible yet
        SDL_FRect roofRect = m_bounds;
        roofRect.y -= 30;
        roofRect.w = 112;
        roofRect.h = 32;
        SDL_RenderCopyF(renderer, m_p_TradingPostTextures[2], NULL, &roofRect);
    }

    positionExplosion();
    m_p_spawnEffect->renderEffect(renderer);
}

