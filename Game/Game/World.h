#pragma once
#include "Vicinity.h"
#include "Body.h"
#include "Entity.h"
#include <list>
#include <iostream>
class Interface;
class Player;
class Enemy;
class World :
    public Vicinity
{
private:
    Vicinity* m_p_topMap_;
    std::unique_ptr<Player> m_p_player_;
    std::list<std::unique_ptr<Enemy>> m_enemyList_;
    std::list<std::unique_ptr<Entity>> m_entityList_;
public:
    World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer);
    ~World();
    World();
    walkingVector checkPlayerMove(int x, int y, double deltaTime);
    void moveWorld(int x, int y, double deltaTime, Interface* p_Interface);
    void renderWorld(SDL_Renderer* renderer, double pixel_per_pixel, Interface* p_Interface);
    void triggerPlayerAttack();
    inline void addVinicityToMap(Vicinity* newVinicity) { m_p_topMap_ = newVinicity; }
    inline void addEntityToMap(Entity* newEntity) { m_entityList_.push_back(std::unique_ptr<Entity>(newEntity)); }
    inline void addEnemyToMap(Enemy* newEnemy) { m_enemyList_.push_back(std::unique_ptr<Enemy>(newEnemy)); }
    inline std::list<std::unique_ptr<Enemy>>* getEnemyList() { return &m_enemyList_; }
    inline std::list<std::unique_ptr<Entity>>* getEntityList() { return &m_entityList_; }
    inline std::unique_ptr<Player>* getPlayer() { return &m_p_player_; }
};

