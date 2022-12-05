#include "World.h"
#include "Interface.h"
#include "Resources.h"
#include "SDL_image.h"
#include "Enemy.h"
#include "Player.h"
World::World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer) : Vicinity(surface, m_bounds_, renderer)
{
	m_p_player_ = std::unique_ptr<Player>(new Player(renderer));

}

World::~World()
{
}

void World::moveWorld(int x, int y, float deltaTime, Interface* p_Interface)
{
	m_p_player_->animateBody(x, y);

	if (m_p_player_->getIsAttacking()) { //No moving while attacking
		x = 0;
		y = 0;
	}
	else {
		walkingVector legalMove = checkPlayerMove(x, y, deltaTime);
		x = legalMove.x;
		y = legalMove.y;
	}

	if (m_p_player_->getIsAttacking()) {
		damageEnemysInPlayerRadius();
	}

	checkForDefeatedEnemies();

	for (auto const& cursor : m_enemyVector_) {
		walkingVector enemyPath = cursor->enemyPathfinding(this, deltaTime);
		cursor->animateBody(enemyPath.x, enemyPath.y);
	}

	if (x == 0 && y == 0) {
		return;
	}
	//------------------------------------------------------- Move the whole world
	for (auto const& cursor : m_enemyVector_) {
		cursor->moveBody(x * deltaTime, y * deltaTime);
	}

	for (auto const& cursor : m_entityVector_) {
		cursor->moveEntity(x * deltaTime, y * deltaTime);
	}

	this->moveVicinity(x * deltaTime, y * deltaTime);
	m_p_topMap_->moveVicinity(x * deltaTime, y * deltaTime);
}

walkingVector World::checkPlayerMove(int x, int y, float deltaTime)
{
	if (!x && !y) {
		return { 0, 0 };
	}

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

void World::renderWorld(SDL_Renderer* renderer, double pixel_per_pixel, Interface* p_Interface)
{
	SDL_RenderClear(renderer);

	int screenHeight, screenWidth;
	SDL_GetWindowSize(p_Interface->getWindow(), &screenWidth, &screenHeight);
	
	//-------------------------------------------- Render the actual level	
	SDL_FRect tmp = m_bounds_;	
	tmp.x = round(tmp.x * pixel_per_pixel);
	tmp.y = round((tmp.y * pixel_per_pixel));
	tmp.w = round(tmp.w * pixel_per_pixel);
	tmp.h = round(tmp.h * pixel_per_pixel);
	
	SDL_RenderCopyF(renderer, m_p_texture_, NULL, &tmp);

	for (auto const& cursor : m_enemyVector_) {
		cursor->renderBody(renderer, pixel_per_pixel);
	}  

	m_p_player_->renderBody(renderer, pixel_per_pixel);

	m_p_topMap_->renderVicinity(renderer, pixel_per_pixel, screenWidth);	//Funktion to render top map
	
	//SDL_FRect* playerTextureCoords = m_p_player_->getSpriteBounds();
	//SDL_FRect attackRadius = { playerTextureCoords->x + 26 * 2, playerTextureCoords->y + 40 * 2, 68 * 2, 40 * 2 };
	//SDL_RenderDrawRectF(renderer, &attackRadius);
}

void World::triggerPlayerAttack()
{
	if (m_p_player_->getIsAttacking())  //Player is already attacking
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
