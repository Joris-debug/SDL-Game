#include <functional>
#include "World.h"
#include "Interface.h"
#include "Resources.h"
#include "SDL_image.h"
#include "Enemy.h"
#include "Player.h"

World::World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer, std::mt19937* m_p_randomNumberEngine_) : Vicinity(surface, m_bounds_, renderer)
{
	m_p_player_ = std::unique_ptr<Player>(new Player(renderer));
	this->m_p_randomNumberEngine_ = m_p_randomNumberEngine_;
}

World::~World()
{
}

void World::moveWorld(float x, float y, float deltaTime)
{

	if (x && y)	{ //If the player moves in 2 directions
		x *= 0.707;
		y *= 0.707;
	}

	m_p_player_->animateBody(x, y);

	if (m_p_player_->getIsAttacking()) { //No moving while attacking
		x = 0;
		y = 0;
		damageEnemysInPlayerRadius();
	}
	else {
		walkingVector legalMove = checkPlayerMove(x, y, deltaTime);
		x = legalMove.x;
		y = legalMove.y;
	}

	checkIfPlayerHit();
	checkForDefeatedEnemies();

	for (auto const& cursor : m_enemyVector_) {
		cursor->enemyPathfinding(this, deltaTime);
	}

	if (!x && !y)
		return;

	//------------------------------------------------------- Move the whole world
	for (auto const& cursor : m_enemyVector_) {
		cursor->moveEntity(x * deltaTime, y * deltaTime);
	}

	for (auto const& cursor : m_entityVector_) {
		cursor->moveEntity(x * deltaTime, y * deltaTime);
	}

	this->moveVicinity(x * deltaTime, y * deltaTime);
	m_p_topMap_->moveVicinity(x * deltaTime, y * deltaTime);
}

walkingVector World::checkPlayerMove(float x, float y, float deltaTime)
{
	if (!x && !y) 
		return { 0, 0 };	

	bool xCollision = false, yCollision = false;
	float xMovement = x * deltaTime * -1, yMovement = y * deltaTime * -1; //Invert because now I am going to move the player and not the world
	SDL_FRect* p_playerBounds = m_p_player_->getFootSpace();

	m_p_player_->moveFootSpace(xMovement, 0);

	for (auto const& cursor : m_entityVector_) {
		if (SDL_HasIntersectionF(p_playerBounds, cursor->getBounds())) {
			xCollision = true;
			break;
		}
	}
	m_p_player_->moveFootSpace(-xMovement, 0);

	m_p_player_->moveFootSpace(0, yMovement);
	for (auto const& cursor : m_entityVector_) {
		if (SDL_HasIntersectionF(p_playerBounds, cursor->getBounds())) {
			yCollision = true;
			break;
		}
	}
	m_p_player_->moveFootSpace(0, -yMovement);

	if (!xCollision && !yCollision) { //check if x and y movement results in colission

		m_p_player_->moveFootSpace(xMovement, yMovement);
		for (auto const& cursor : m_entityVector_) {
			if (SDL_HasIntersectionF(p_playerBounds, cursor->getBounds())) {
				yCollision = true;
				break;
			}
		}
		m_p_player_->moveFootSpace(-xMovement, -yMovement);
	}
	return{ !xCollision * x, !yCollision * y };
}

void World::renderWorld(SDL_Renderer* renderer)
{
	//-------------------------------------------- Render the actual level	

	SDL_RenderCopyF(renderer, m_p_texture_, NULL, &m_bounds_);

	for (auto const& cursor : m_enemyVector_) {
		cursor->renderBody(renderer);
		//SDL_FRect* playerTextureCoords = cursor->getBounds();
		//SDL_RenderDrawRectF(renderer, playerTextureCoords);
	}  

	m_p_player_->renderBody(renderer);
	m_p_topMap_->renderVicinity(renderer);	//Funktion to render top map
	
	//SDL_FRect* playerTextureCoords = m_entityVector_[0]->getBounds();
	//SDL_RenderDrawRectF(renderer, playerTextureCoords);

}

void World::triggerPlayerAttack()
{
	const float playerAttackCooldown = PLAYER_ATTACK_COOLDOWN;
	if (SDL_GetTicks() - m_p_player_->getLastAttack() < playerAttackCooldown || m_p_player_->isInvincible())  //Player is already attacking or Attacked less than 8 seconds ago
		return;	
	else
		m_p_player_->setIsAttacking();
}

void World::damageEnemysInPlayerRadius()
{
	SDL_FRect* playerTextureCoords = m_p_player_->getSpriteBounds();
	SDL_FRect attackRadius = { playerTextureCoords->x + 26*2, playerTextureCoords->y + 40*2, 68*2, 40*2 };
	auto it = m_enemyVector_.begin();
	while (it != m_enemyVector_.end()) {
		if (SDL_HasIntersectionF(&attackRadius, it->get()->getBounds())) {
			it->get()->damageBody(1);			
		}
		it++;		
	}
}

void World::checkIfPlayerHit()
{
	auto it = m_enemyVector_.begin();
	while (it != m_enemyVector_.end()) {
		if (SDL_HasIntersectionF(m_p_player_->getBounds(), it->get()->getBounds())) {
			m_p_player_->damageBody(1);
		}
		it++;
	}
}

void World::checkForDefeatedEnemies()
{
	auto it = m_enemyVector_.begin();
	while (it != m_enemyVector_.end()) {
		if (it->get()->getCurrentLives() == 0 && !it->get()->isInvincible()) {
			m_enemyVector_.erase(it);
			it--;
		}
		it++;
	}
}

SDL_FPoint World::getRandomCoordinate()
{
	int randomXCoordinate = getRandomNumber(m_bounds_.x + 32, m_bounds_.x + m_bounds_.w - 64);
	int randomYCoordinate = getRandomNumber(m_bounds_.y + 32, m_bounds_.y + m_bounds_.h - 64);
	return { float(randomXCoordinate), float(randomYCoordinate) };
}

int World::getRandomNumber(int rangeBegin, int rangeEnde)
{
	std::uniform_int_distribution<int> distribution(rangeBegin, rangeEnde);
	return (distribution(*m_p_randomNumberEngine_));
}
