#pragma once
#include "SDL.h"
class GameHandler;
class Vicinity
{
protected:
	SDL_Texture* m_p_texture;
	SDL_FRect m_bounds;

public:
	Vicinity(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer);
	virtual ~Vicinity();
	void moveVicinity(float x, float y);
	void renderVicinity(SDL_Renderer* renderer);
	inline SDL_FRect* getBounds() { return &m_bounds; }
	inline SDL_Texture* getTexture() { return m_p_texture; }
};

