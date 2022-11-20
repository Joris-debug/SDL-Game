#include "Enemy.h"

Enemy::Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_) : Body(m_bounds_, m_spriteBounds_)
{
	this->m_p_textureIdle_ = m_p_textureIdle_;
	this->m_p_textureRun_ = m_p_textureRun_;
}

walkingVector Enemy::enemyPathfinding()
{
	walkingVector tmp { 0, 0 };
	return tmp;
}

void Enemy::animateBody(int x, int y)
{
	int totalSprites = 1;
	int delayPerFrame = 100;
	int spriteLayer = 0;

	if (!x && !y) {
		delayPerFrame = 1500;
		m_currentMode_ = 1;
		totalSprites = 4;
	}

	int tmp = 32 * ((SDL_GetTicks() / delayPerFrame) % totalSprites);
	m_spriteCoords_ = { tmp, 32 * spriteLayer,  32,  32 };
}

void Enemy::renderBody(SDL_Renderer* renderer, double pixel_per_pixel)
{
	SDL_FRect tmp = m_spriteBounds_;
	tmp.x = round(tmp.x * pixel_per_pixel);
	tmp.y = round(tmp.y * pixel_per_pixel);
	tmp.w = round(tmp.w * pixel_per_pixel);
	tmp.h = round(tmp.h * pixel_per_pixel);

	switch (m_currentMode_) {
	case 1:
		SDL_RenderCopyF(renderer, m_p_textureIdle_, &m_spriteCoords_, &tmp);
		break;
	}
}

Enemy::~Enemy()
{
}
