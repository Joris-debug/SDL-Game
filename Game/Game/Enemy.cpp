#include "Enemy.h"
#include "World.h"
#include "Player.h"
Enemy::Enemy(SDL_Texture* m_p_textureIdle_, SDL_Texture* m_p_textureRun_, SDL_FRect m_bounds_, SDL_FRect m_spriteBounds_) : Body(m_bounds_, m_spriteBounds_)
{
	this->m_p_textureIdle_ = m_p_textureIdle_;
	this->m_p_textureRun_ = m_p_textureRun_;
}

walkingVector Enemy::enemyPathfinding(World* p_world, double deltaTime)
{
	Player* enemyTarget = p_world->getPlayer()->get();
	SDL_FRect targetLocation = enemyTarget->getBounds();

	int x = 0, y = 0;
	int margin = 8;

	if (targetLocation.x + (targetLocation.w / 2.0) > m_bounds_.x + (m_bounds_.w / 2.0) + margin)
		x = 1;
	else if (targetLocation.x + (targetLocation.w / 2.0) + margin < m_bounds_.x + (m_bounds_.w / 2.0))
		x = -1;

	if (targetLocation.y + (targetLocation.h / 2.0) > m_bounds_.y + (m_bounds_.h / 2.0) + margin)
		y = 1;
	else if (targetLocation.y + (targetLocation.h / 2.0) + margin < m_bounds_.y + (m_bounds_.h / 2.0))
		y = -1;

	bool xCollision = false, yCollision = false;
	if (x != 0) {
		this->moveEntity(x * deltaTime, 0);
		for (auto const& cursor : *p_world->getEnemyList()) {
			if (SDL_HasIntersectionF(this->m_bounds_, cursor->getBounds())) {

			}
		}

		
	}
	return { x * !xCollision, y * !yCollision };
}

void Enemy::animateBody(int x, int y)
{
	int totalSprites = 1;
	int delayPerFrame = 100;
	int spriteLayer = 0;

	if (!x && !y) {
		delayPerFrame = 6000;
		m_currentMode_ = 1;
		totalSprites = 4;
	}

	else if (x == 1) {
		m_currentMode_ = 2;
		totalSprites = 4;
		spriteLayer = 1;
	}

	else if (x == -1) {
		m_currentMode_ = 3;
		totalSprites = 4;
		spriteLayer = 2;
	}

	else if (y == 1) {
		m_currentMode_ = 4;
		totalSprites = 4;
		spriteLayer = 0;
	}

	else if (y == -1) {
		m_currentMode_ = 4;
		totalSprites = 4;
		spriteLayer = 3;
	}

	int tmp = 32 * ((SDL_GetTicks() / delayPerFrame) % totalSprites);
	m_textureCoords_ = { tmp, 32 * spriteLayer,  32,  32 };
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
		SDL_RenderCopyF(renderer, m_p_textureIdle_, &m_textureCoords_, &tmp);
		break;
	case 2:
	case 3:
	case 4:
		SDL_RenderCopyF(renderer, m_p_textureRun_, &m_textureCoords_, &tmp);
		break;
	}
}

Enemy::~Enemy()
{
	SDL_DestroyTexture(m_p_textureIdle_);
	SDL_DestroyTexture(m_p_textureRun_);
}
