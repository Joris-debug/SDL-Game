#include <functional>
#include "World.h"
#include "Interface.h"
#include "Resources.h"
#include "SDL_image.h"
#include "Enemy.h"
#include "Player.h"
#include "TradingPost.h"

World::World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer, std::mt19937* m_p_randomNumberEngine_) : Vicinity(surface, m_bounds_, renderer)
{
	m_p_player_ = std::unique_ptr<Player>(new Player(renderer));
	this->m_p_randomNumberEngine_ = m_p_randomNumberEngine_;
	m_merchantIsActive_ = false;
	m_p_merchant_ = new TradingPost(renderer, m_p_randomNumberEngine_);
}

World::~World()
{
	delete m_p_merchant_;

	int numberOfElements = m_entityVector_.size();
	for (int i = 0; i < numberOfElements; i++) {
		delete m_entityVector_.back();
		m_entityVector_.pop_back();
	}
}

void World::moveWorld(float x, float y, float deltaTime)
{
	if (x && y)	{ //If the player moves in 2 directions
		x *= 0.707f;
		y *= 0.707f;
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

	for (auto cursor : m_entityVector_) {
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

	for (auto cursor : m_entityVector_) {
		if (SDL_HasIntersectionF(p_playerBounds, cursor->getBounds())) {
			xCollision = true;
			break;
		}
	}
	m_p_player_->moveFootSpace(-xMovement, 0);

	m_p_player_->moveFootSpace(0, yMovement);
	for (auto cursor : m_entityVector_) {
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

	if(m_merchantIsActive_)
		m_p_merchant_->renderTradingPost(renderer);

	for (auto const& cursor : m_enemyVector_) {
		cursor->renderBody(renderer);
		//SDL_FRect* playerTextureCoords = cursor->getBounds();
		//SDL_RenderDrawRectF(renderer, playerTextureCoords);
	}  

	m_p_player_->renderBody(renderer);

	if(m_merchantIsActive_)
		m_p_merchant_->renderTradingPostRoof(renderer);

	m_p_topMap_->renderVicinity(renderer);	//Funktion to render top map
	
	//SDL_FRect* playerTextureCoords = m_entityVector_[0]->getBounds();
	//SDL_RenderDrawRectF(renderer, playerTextureCoords);

}

void World::triggerPlayerAttack()
{
	if (!m_p_player_->checkAttackCooldown() || m_p_player_->isInvincible())  //Player is already attacking or Attacked less than 8 seconds ago
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
	int enemyCount = m_enemyVector_.size();
	if (!enemyCount)
		return;

	auto it = m_enemyVector_.begin();
	while (it != m_enemyVector_.end()) {
		if (it->get()->getCurrentLives() == 0 && !it->get()->isInvincible()) {
			m_enemyVector_.erase(it);
			it--;
		}
		it++;
	}

	if (enemyCount && m_enemyVector_.size() == 0)	//Enemys exist at the start of the function, but now the vector is empty
		makeMerchantAppear();
}

void World::makeMerchantAppear()
{
	m_merchantIsActive_ = true;

	bool successfullSpawn = false;

	for (int i = 0; i < 30; i++) {
		std::cout << "trying to spawn\n";
		if (trySpawningMerchantClose()) {
			successfullSpawn = true;
			break;
		}
	}

	while (!successfullSpawn) {
		if (trySpawningMerchantFar())
			successfullSpawn = true;
	}

	addEntityToMap(m_p_merchant_);
}

bool World::trySpawningMerchantClose()
{
	SDL_FRect* p_merchantBounds = m_p_merchant_->getBounds();
	p_merchantBounds->x = getRandomNumber(0, 800);
	p_merchantBounds->y = getRandomNumber(0, 640);

	if (!SDL_HasIntersectionF(p_merchantBounds, &m_bounds_)) {
		return false;
	}

	for (auto const& cursor : m_enemyVector_) {
		if (SDL_HasIntersectionF(p_merchantBounds, cursor->getBounds())) {
			return false;
		}
	}

	for (auto cursor : m_entityVector_) {
		if (SDL_HasIntersectionF(p_merchantBounds, cursor->getBounds())) {
			return false;
		}
	}

	return true;
}

bool World::trySpawningMerchantFar()
{
	SDL_FRect* p_merchantBounds = m_p_merchant_->getBounds();
	SDL_FPoint randomPosition = getRandomCoordinate();

	p_merchantBounds->x = randomPosition.x;
	p_merchantBounds->y = randomPosition.y;

	for (auto const& cursor : m_enemyVector_) {
		if (SDL_HasIntersectionF(p_merchantBounds, cursor->getBounds())) {
			return false;
		}
	}

	for (auto cursor : m_entityVector_) {
		if (SDL_HasIntersectionF(p_merchantBounds, cursor->getBounds())) {
			return false;
		}
	}

	return true;
}

void World::despawnMerchant()
{
	m_merchantIsActive_ = false;

	auto it = m_entityVector_.begin();
	while (it != m_entityVector_.end()) {
		if (typeid(**it) == typeid(TradingPost)) {
			m_entityVector_.erase(it);
			break;
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
