#pragma once
#include "Vicinity.h"
#include <list>
class World :
    public Vicinity
{
private:
    std::list<Vicinity*> m_listOfLayers_;
public:
    World(SDL_Surface* surface, SDL_Rect m_bounds_, SDL_Renderer *renderer);
    void moveWorld(int x, int y);
    void renderWorld(SDL_Renderer* renderer, double pixel_per_pixel);
    inline void addVinicityToMap(Vicinity* newVinicity) { m_listOfLayers_.push_back(newVinicity); }
};

