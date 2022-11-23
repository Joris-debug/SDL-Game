#include "World.h"
#include "Interface.h"
#include "Resources.h"
#include "SDL_image.h"
#include "Enemy.h"
#include "Player.h"
World::World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer) : Vicinity(surface, m_bounds_, renderer)
{
	m_p_player_ = std::unique_ptr<Player>(new Player(renderer));

	SDL_Surface* tmpSurfaceIdle = IMG_Load(RSC_MANTIS_IDLE);
	SDL_Surface* tmpSurfaceWalk = IMG_Load(RSC_MANTIS_WALK);
	m_enemyList_.push_back(std::unique_ptr<Enemy>(new Enemy(SDL_CreateTextureFromSurface(renderer, tmpSurfaceIdle), SDL_CreateTextureFromSurface(renderer, tmpSurfaceWalk), {100, 100, 64, 64}, { 100, 100, 64, 64 })));
	SDL_FreeSurface(tmpSurfaceIdle);
	SDL_FreeSurface(tmpSurfaceWalk);


	tmpSurfaceIdle = IMG_Load(RSC_MAGGOT_IDLE);
	tmpSurfaceWalk = IMG_Load(RSC_MAGGOT_WALK);
	m_enemyList_.push_back(std::unique_ptr<Enemy>(new Enemy(SDL_CreateTextureFromSurface(renderer, tmpSurfaceIdle), SDL_CreateTextureFromSurface(renderer, tmpSurfaceWalk), { -100, -100, 64, 64 }, { -100, -100, 64, 64 })));
	SDL_FreeSurface(tmpSurfaceIdle);
	SDL_FreeSurface(tmpSurfaceWalk);
}

World::~World()
{
}

World::World()
{
}

void World::moveWorld(int x, int y, double deltaTime, Interface* p_Interface)
{
	m_p_player_->animateBody(x, y);

	for (auto const& cursor : m_enemyList_) {
		walkingVector enemyPath = cursor->enemyPathfinding(this, deltaTime);
		cursor->animateBody(enemyPath.x, enemyPath.y);
		//cursor->moveBody(enemyPath.x * deltaTime * 0.4, enemyPath.y * deltaTime * 0.4);
	}

	if (x == 0 && y == 0) {
		return;
	}
	
	for (auto const& cursor : m_enemyList_) {
		cursor->moveBody(x * deltaTime, y * deltaTime);
	}

	this->moveVicinity(x * deltaTime, y * deltaTime);

	int screenWidth;
	int screenHeight;
	SDL_GetWindowSize(p_Interface->getWindow(), &screenWidth, &screenHeight);

	//m_p_topMap_->moveVicinity(x * deltaTime, y * deltaTime);
}

void World::renderWorld(SDL_Renderer* renderer, double pixel_per_pixel, Interface* p_Interface)
{
	SDL_RenderClear(renderer);
	int screenWidth;
	int screenHeight;

	SDL_GetWindowSize(p_Interface->getWindow(), &screenWidth, &screenHeight);

	//m_p_topMap_->renderVicinity(renderer, pixel_per_pixel, screenWidth);	//Funktion to render top map
	
	//Render the actual level
	
	SDL_FRect tmp = m_bounds_;	
	tmp.x = round(tmp.x * pixel_per_pixel);
	tmp.y = round((tmp.y * pixel_per_pixel));
	tmp.w = round(tmp.w * pixel_per_pixel);
	tmp.h = round(tmp.h * pixel_per_pixel);
	
	SDL_RenderCopyF(renderer, m_p_texture_, NULL, &tmp);

	for (auto const& cursor : m_enemyList_) {
		cursor->renderBody(renderer, pixel_per_pixel);
	}

	m_p_player_->renderBody(renderer, pixel_per_pixel);
	//SDL_FRect tmpB = m_enemyList_.front()->getBounds();
	//SDL_RenderDrawRectF(renderer, &tmpB);

}
