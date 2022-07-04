#pragma once
#include "SDL.h"
class GameHandler;
class Vicinity
{
private:
	SDL_Texture* m_p_texture_;
	SDL_Rect m_bounds_;

public:
	Vicinity(SDL_Surface* surface, SDL_Rect m_bounds_, SDL_Renderer* renderer);
	~Vicinity();
	void moveVicinty(int x, int y);
	void renderVicinity(SDL_Renderer* renderer, double pixel_per_pixel);
};

