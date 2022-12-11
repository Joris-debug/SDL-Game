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
		//SDL_FRect* playerTextureCoords = cursor->getBounds();
		//SDL_RenderDrawRectF(renderer, playerTextureCoords);
	}  

	m_p_player_->renderBody(renderer, pixel_per_pixel);

	m_p_topMap_->renderVicinity(renderer, pixel_per_pixel, screenWidth);	//Funktion to render top map
	
	//SDL_FRect* playerTextureCoords = m_p_player_->getBounds();
	//SDL_RenderDrawRectF(renderer, playerTextureCoords);
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

SDL_FPoint World::getRandomCoordinate()
{
	int randomXCoordinate = getRandomNumber(m_bounds_.x, m_bounds_.x + m_bounds_.w);
	int randomYCoordinate = getRandomNumber(m_bounds_.y, m_bounds_.x + m_bounds_.h);
	return { float(randomXCoordinate), float(randomYCoordinate) };
}

int World::getRandomNumber(int rangeBegin, int rangeEnde)
{
	std::uniform_int_distribution<int> distribution(rangeBegin, rangeEnde);
	return (distribution(*m_p_randomNumberEngine_));
}

int World::computeCodeForCohenSutherland(SDL_FRect* rectangle, float x, float y)
{
	const int INSIDE = 0; // 0000
	const int LEFT = 1; // 0001
	const int RIGHT = 2; // 0010
	const int BOTTOM = 4; // 0100
	const int TOP = 8; // 1000

	int code = INSIDE;	// initialized as being inside	

	float x_max = rectangle->x + rectangle->w;
	float y_max = rectangle->y + rectangle->h;
	float x_min = rectangle->x;
	float y_min = rectangle->y;

	if (x < x_min) // to the left of rectangle
		code |= LEFT;
	else if (x > x_max) // to the right of rectangle
		code |= RIGHT;
	if (y < y_min) // below the rectangle
		code |= BOTTOM;
	else if (y > y_max) // above the rectangle
		code |= TOP;

	return code;
}

bool World::IntersectEntityAndLine(SDL_FRect* rectangle, SDL_FPoint point1, SDL_FPoint point2)
{
	const int INSIDE = 0; // 0000
	const int LEFT = 1; // 0001
	const int RIGHT = 2; // 0010
	const int BOTTOM = 4; // 0100
	const int TOP = 8; // 1000

	float x1 = point1.x;
	float y1 = point1.y;
	float x2 = point2.x;
	float y2 = point2.y;
	
	// Compute region codes for P1, P2
	int code1 = computeCodeForCohenSutherland(rectangle, x1, y1);
	int code2 = computeCodeForCohenSutherland(rectangle, x2, y2);

	float x_max = rectangle->x + rectangle->w;
	float y_max = rectangle->y + rectangle->h;
	float x_min = rectangle->x;
	float y_min = rectangle->y;

	// Initialize line as outside the rectangular window
	bool accept = false;

	while (true) {
		if ((code1 == 0) && (code2 == 0)) {
			// If both endpoints lie within rectangle
			accept = true;
			break;
		}
		else if (code1 & code2) {
			// If both endpoints are outside rectangle,
			// in same region
			break;
		}
		else {
			// Some segment of line lies within the
			// rectangle
			int code_out;
			double x, y;

			// At least one endpoint is outside the
			// rectangle, pick it.
			if (code1 != 0)
				code_out = code1;
			else
				code_out = code2;

			// Find intersection point;
			// using formulas y = y1 + slope * (x - x1),
			// x = x1 + (1 / slope) * (y - y1)
			if (code_out & TOP) {
				// point is above the clip rectangle
				x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1);
				y = y_max;
			}
			else if (code_out & BOTTOM) {
				// point is below the rectangle
				x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1);
				y = y_min;
			}
			else if (code_out & RIGHT) {
				// point is to the right of rectangle
				y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1);
				x = x_max;
			}
			else if (code_out & LEFT) {
				// point is to the left of rectangle
				y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1);
				x = x_min;
			}

			// Now intersection point x, y is found
			// We replace point outside rectangle
			// by intersection point
			if (code_out == code1) {
				x1 = x;
				y1 = y;
				code1 = computeCodeForCohenSutherland(rectangle, x1, y1);
			}
			else {
				x2 = x;
				y2 = y;
				code2 = computeCodeForCohenSutherland(rectangle, x2, y2);
			}
		}
	}

	if (accept)
		return false;
	else
		return true;
}
