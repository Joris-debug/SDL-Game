#include <functional>
#include "World.h"
#include "Interface.h"
#include "Resources.h"
#include "SDL_image.h"
#include "Beetle.h"
#include "VirtualEnemy.h"
#include "Player.h"
#include "PlayerTwo.h"
#include "Effect.h"
#include "MenuManager.h"
#include "TradingPost.h"
#include "SoundHandler.h"

World::World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer, std::mt19937* m_p_randomNumberEngine_, Effect* m_p_spawnEffect_) : Vicinity(surface, m_bounds_, renderer)
{
	m_p_player = new Player(renderer);
	m_p_playerTwo = nullptr; //The map is always initiated with just one player
	this->m_p_randomNumberEngine = m_p_randomNumberEngine_;
	m_merchantIsActive = false;
	m_p_merchant = new TradingPost(renderer, m_p_randomNumberEngine_, m_p_spawnEffect_);
	m_serverLock = false;
	m_p_topMap = nullptr;
	m_triggerMerchantSpawnOnClient = false;	//I dont want the server to tell the client that it should spawn a merchant yet
}

World::~World()
{

	int numberOfElements = int(m_entityVector.size());
	for (int i = 0; i < numberOfElements; i++) {
		if(m_entityVector.back() != m_p_merchant)	//The merchant can be inside this list
			delete m_entityVector.back();
		m_entityVector.pop_back();
	}

	numberOfElements = int(m_enemyVector.size());
	for (int i = 0; i < numberOfElements; i++) {
		delete m_enemyVector.back();
		m_enemyVector.pop_back();
	}

	delete m_p_topMap;
	m_p_topMap = nullptr;
	delete m_p_merchant;
	m_p_merchant = nullptr;
	delete m_p_player;
	m_p_player = nullptr;
}

void World::moveWorld(float x, float y, float deltaTime)
{
	if (x && y)	{ //If the player moves in 2 directions
		x *= 0.707f;
		y *= 0.707f;
	}

	m_p_player->animateBody(x, y);

	if (m_p_player->getIsAttacking()) { //No moving while attacking
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

	if (m_p_playerTwo) {
		if(m_p_playerTwo->isAttacking())
			damageEnemysInPlayerTwoRadius();
		checkIfPlayerTwoHit();
	}

	checkForDefeatedEnemies();
	checkIfMerchantDespawned();

	for (auto const& cursor : m_enemyVector) {
		cursor->enemyPathfinding(this, deltaTime);
	}

	if (!x && !y)
		return;

	//------------------------------------------------------- Move the whole world
	for (auto const& cursor : m_enemyVector) {
		cursor->moveEntity(x * deltaTime, y * deltaTime);
	}

	for (auto cursor : m_entityVector) {
		cursor->moveEntity(x * deltaTime, y * deltaTime);
	}

	if (m_p_playerTwo)
		m_p_playerTwo->moveEntity(x * deltaTime, y * deltaTime);

	this->moveVicinity(x * deltaTime, y * deltaTime);
	m_p_topMap->moveVicinity(x * deltaTime, y * deltaTime);
}

walkingVector World::checkPlayerMove(float x, float y, float deltaTime)
{
	if (!x && !y) 
		return { 0, 0 };	

	bool xCollision = false, yCollision = false;
	float xMovement = x * deltaTime * -1, yMovement = y * deltaTime * -1; //Invert because now I am going to move the player and not the world
	SDL_FRect* p_playerBounds = m_p_player->getFootSpace();
	SDL_FRect* p_playerTwoBounds = (m_p_playerTwo) ? m_p_playerTwo->getFootSpace() : nullptr;
	bool playersOverlap = (m_p_playerTwo) ? SDL_HasIntersectionF(p_playerBounds, p_playerTwoBounds) : true;

	m_p_player->moveFootSpace(xMovement, 0);

	for (auto cursor : m_entityVector) {
		if (SDL_HasIntersectionF(p_playerBounds, cursor->getBounds())) {
			xCollision = true;
			break;
		}
	}

	if (!xCollision && !playersOverlap && SDL_HasIntersectionF(p_playerBounds, p_playerTwoBounds)) {
		xCollision = true;
	}

	m_p_player->moveFootSpace(-xMovement, 0);

	m_p_player->moveFootSpace(0, yMovement);
	for (auto cursor : m_entityVector) {
		if (SDL_HasIntersectionF(p_playerBounds, cursor->getBounds())) {
			yCollision = true;
			break;
		}
	}

	if (!yCollision && !playersOverlap && SDL_HasIntersectionF(p_playerBounds, p_playerTwoBounds)) {
		yCollision = true;
	}

	m_p_player->moveFootSpace(0, -yMovement);

	if (!xCollision && !yCollision) { //check if x and y movement results in colission

		m_p_player->moveFootSpace(xMovement, yMovement);
		for (auto const& cursor : m_entityVector) {
			if (SDL_HasIntersectionF(p_playerBounds, cursor->getBounds())) {
				yCollision = true;
				break;
			}
		}

		if (!yCollision && !playersOverlap && SDL_HasIntersectionF(p_playerBounds, p_playerTwoBounds)) {
			yCollision = true;
		}

		m_p_player->moveFootSpace(-xMovement, -yMovement);
	}

	return{ !xCollision * x, !yCollision * y };
}

void World::renderWorld(SDL_Renderer* renderer)
{
	//-------------------------------------------- Render the actual level	

	SDL_RenderCopyF(renderer, m_p_texture, NULL, &m_bounds);

	if(m_merchantIsActive)
		m_p_merchant->renderTradingPost(renderer);

	for (auto cursor : m_enemyVector) {
		if(cursor->getEnemyType() != EnemyType::beetle)
			cursor->renderBody(renderer);
	}

	if (m_p_playerTwo) {
		if (m_p_playerTwo->getBounds()->y < m_p_player->getBounds()->y) {  //If there is a playerTwo and he is positioned above player one
			m_p_playerTwo->renderBody(renderer);
			m_p_player->renderBody(renderer);
		}
		else {
			m_p_player->renderBody(renderer);
			m_p_playerTwo->renderBody(renderer);
		}
	}
	else
		m_p_player->renderBody(renderer);
	

	if(m_merchantIsActive)
		m_p_merchant->renderTradingPostRoof(renderer);

	m_p_topMap->renderVicinity(renderer);	//Funktion to render top map
	
	if (talkToMerchant())
		m_p_merchant->renderAlert(renderer);

	for (auto cursor : m_enemyVector) {
		if (cursor->getEnemyType() == EnemyType::beetle)
			cursor->renderBody(renderer);
	}

	//SDL_FRect* playerTextureCoords = m_p_player->getFootSpace();
	//SDL_RenderDrawRectF(renderer, playerTextureCoords);

	//playerTextureCoords = m_p_playerTwo->getFootSpace();
	//SDL_RenderDrawRectF(renderer, playerTextureCoords);

	//for (auto cursor : m_enemyVector) {
	//	SDL_RenderDrawRectF( renderer, cursor->getBounds());
	//}

}

void World::triggerPlayerAttack()
{
	if (!m_p_player->checkAttackCooldown() || m_p_player->isInvincible())  //Player is already attacking or Attacked less than 8 seconds ago
		return;
	SoundHandler::getInstance().playSwordSound();
	m_p_player->setIsAttacking();
}

void World::damageEnemysInPlayerRadius()
{
	if (m_enemyVector.empty())
		return;
	SDL_FRect* playerTextureCoords = m_p_player->getSpriteBounds();
	SDL_FRect attackRadius = { playerTextureCoords->x + 26*2, playerTextureCoords->y + 40*2, 68*2, 41*2 };
	auto it = m_enemyVector.begin();
	while (it != m_enemyVector.end()) {
		if (SDL_HasIntersectionF(&attackRadius, (*it)->getBounds()) && (*it)->damageBody(1)) {
			SoundHandler::getInstance().playEnemyHitSound();
		}
		it++;		
	}
}

void World::damageEnemysInPlayerTwoRadius()
{
	if (m_enemyVector.empty())
		return;
	SDL_FRect* playerTextureCoords = m_p_playerTwo->getSpriteBounds();
	SDL_FRect attackRadius = { playerTextureCoords->x + 26 * 2, playerTextureCoords->y + 40 * 2, 68 * 2, 41 * 2 };
	auto it = m_enemyVector.begin();
	while (it != m_enemyVector.end()) {
		if (SDL_HasIntersectionF(&attackRadius, (*it)->getBounds()) && (*it)->damageBody(1)) {
			SoundHandler::getInstance().playEnemyHitSound();
		}
		it++;
	}
}

void World::checkIfPlayerHit()
{
	if (m_enemyVector.empty())
		return;

	auto it = m_enemyVector.begin();
	if (typeid(**it) == typeid(VirtualEnemy))	//VirtualEnemies dont damage the player
			return;

	while (it != m_enemyVector.end()) {

		if (SDL_HasIntersectionF(m_p_player->getBounds(), (*it)->getBounds()) && m_p_player->damageBody(1)) {
			SoundHandler::getInstance().playPlayerHitSound();
		}
		it++;
	}
}

void World::checkIfPlayerTwoHit()
{
	if (!m_p_playerTwo->getPlayerType())		//Return value 1 means that player two is the client and i am the server, thats why i have to check for damage
		return;

	auto it = m_enemyVector.begin();
	while (it != m_enemyVector.end()) {
		if (SDL_HasIntersectionF(m_p_playerTwo->getBounds(), (*it)->getBounds()) && m_p_playerTwo->damageBody(1)) {
			SoundHandler::getInstance().playPlayerHitSound();
			m_p_playerTwo->setHitDetected(true);
		}
		it++;
	}
}

void World::checkForDefeatedEnemies()
{
	int enemyCount = int(m_enemyVector.size());
	if (!enemyCount)
		return;

	auto it = m_enemyVector.begin();

	if (typeid(**it) == typeid(VirtualEnemy))	//VirtualEnemies wont be deleted in this function
		return;

	while (it != m_enemyVector.end()) {
		if ((*it)->getCurrentLives() == 0 && !(*it)->isInvincible()) {
			while (m_serverLock);	//Wait for the GameHandler to finish transmitting
			m_serverLock = true;
			delete *it;
			m_enemyVector.erase(it);
			it--;
			m_serverLock = false;
		}
		it++;
	}

	if (enemyCount && m_enemyVector.size() == 0)	//Enemys existed at the start of the function, but now the vector is empty
		makeMerchantAppear();
}

void World::makeMerchantAppear()
{
	if (m_merchantIsActive)
		return;

	m_triggerMerchantSpawnOnClient = true;	//I will only look at this value if there is a server thread running in the background

	m_merchantIsActive = true;

	bool successfullSpawn = false;

	for (int i = 0; i < 30; i++) {
		if (trySpawningMerchantClose()) {
			successfullSpawn = true;
			break;
		}
	}

	while (!successfullSpawn) {
		if (trySpawningMerchantFar())
			successfullSpawn = true;
	}
	m_p_merchant->setIsActive(true);
	m_p_merchant->getSpawnEffect()->setEffectIsDone(false); //Start the explosion again
	SoundHandler::getInstance().playExplosionSound();
	addEntityToMap(m_p_merchant);
}

bool World::trySpawningMerchantClose()
{
	SDL_FRect* p_merchantBounds = m_p_merchant->getBounds();
	p_merchantBounds->x = getRandomNumber(0, 800);
	p_merchantBounds->y = getRandomNumber(0, 640);

	if (!SDL_HasIntersectionF(p_merchantBounds, &m_bounds)) {
		return false;
	}

	for (auto cursor : m_entityVector) {
		if (SDL_HasIntersectionF(p_merchantBounds, cursor->getBounds())) {
			return false;
		}
	}

	if (SDL_HasIntersectionF(p_merchantBounds, m_p_player->getBounds())) {
		return false;
	}

	return true;
}

bool World::trySpawningMerchantFar()
{
	SDL_FRect* p_merchantBounds = m_p_merchant->getBounds();
	SDL_FPoint randomPosition = getRandomCoordinate();

	p_merchantBounds->x = randomPosition.x;
	p_merchantBounds->y = randomPosition.y;

	for (auto cursor : m_entityVector) {
		if (SDL_HasIntersectionF(p_merchantBounds, cursor->getBounds())) {
			return false;
		}
	}

	if (SDL_HasIntersectionF(p_merchantBounds, m_p_player->getBounds())) {
		return false;
	}

	return true;
}

bool World::talkToMerchant()
{
	if (!m_p_merchant->getIsActive())
		return false;
	
	if (SDL_HasIntersectionF(m_p_merchant->getBounds(), m_p_player->getSpriteBounds())) {
		return true;
	}		
	return false;
}

void World::sendMerchantAway()
{
	if (!m_p_merchant->getIsActive())
		return;
	SoundHandler::getInstance().playClickSound();
	m_p_merchant->getSpawnEffect()->setEffectIsDone(false); //Start the explosion again
	SoundHandler::getInstance().playExplosionSound();
	m_p_merchant->setIsActive(false);	//This lets the merchant know that its being despawned
}

bool World::checkIfMerchantDespawned()
{
	if (!m_merchantIsActive)	//There is no merchant
		return false;

	if (!m_p_merchant->getSpawnEffect()->getEffectIsDone() || m_p_merchant->getIsActive())	//If the animation is not done or if the merchant is still active
		return false;

	m_merchantIsActive = false;	//The merchant can now be 100% removed from the world, every animation has played

	auto it = m_entityVector.begin();
	while (it != m_entityVector.end()) {
		if (typeid(**it) == typeid(TradingPost)) {
			m_entityVector.erase(it);
			break;
		}
		it++;
	}

	return true;
}

SDL_FPoint World::getRandomCoordinate()
{
	int randomXCoordinate = getRandomNumber(round(m_bounds.x) + 32, round(m_bounds.x + m_bounds.w) - 64);
	int randomYCoordinate = getRandomNumber(round(m_bounds.y) + 32, round(m_bounds.y + m_bounds.h) - 64);
	return { float(randomXCoordinate), float(randomYCoordinate) };
}

int World::getRandomNumber(int rangeBegin, int rangeEnde)
{
	std::uniform_int_distribution<int> distribution(rangeBegin, rangeEnde);
	return (distribution(*m_p_randomNumberEngine));
}

bool World::checkIfEnemyExists(int enemyId)
{
	for(auto cursor : m_enemyVector)
		if(cursor->getEnemyId() == enemyId)
			return true;
	return false;
}

Enemy* World::getEnemyById(int enemyId)
{
	for (auto cursor : m_enemyVector) {
		if (cursor->getEnemyId() == enemyId)
			return cursor;
	}
	return nullptr;
}

void World::deleteNotExistingVirtualEnemies(std::vector<int> existingEnemies)
{
	auto it = m_enemyVector.begin();
	while (it != m_enemyVector.end()) {
		bool foundEnemy = false;
		int enemyId = (*it)->getEnemyId();
		for (auto cursorId : existingEnemies) {
			if (enemyId == cursorId)
				foundEnemy = true;
		}
		if (!foundEnemy) {
			delete* it;
			m_enemyVector.erase(it);
			it--;
		}
		it++;
	}

}
