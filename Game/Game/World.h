#pragma once
#include "Vicinity.h"
#include <list>
#include <iostream>
class Interface;
class Player;
class World :
    public Vicinity
{
private:
    Vicinity* m_p_topMap_;
    std::unique_ptr<Player> m_p_player_;
public:
    World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer *renderer);
    ~World();
    World();
    void moveWorld(int x, int y, double deltaTime, Interface* p_Interface);
    void renderWorld(SDL_Renderer* renderer, double pixel_per_pixel, Interface* p_Interface);
    inline void addVinicityToMap(Vicinity* newVinicity) { m_p_topMap_ = newVinicity; }
};

