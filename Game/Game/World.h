#pragma once
#include "Vicinity.h"
#include "Body.h"
#include "Entity.h"
#include <vector>
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
    std::vector<std::unique_ptr<Enemy>> m_enemyVector_;
    std::vector<std::unique_ptr<Entity>> m_entityVector_;
public:
    World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer);
    ~World();
    walkingVector checkPlayerMove(int x, int y, float deltaTime);
    void moveWorld(int x, int y, float deltaTime, Interface* p_Interface);
    void renderWorld(SDL_Renderer* renderer, double pixel_per_pixel, Interface* p_Interface);
    void triggerPlayerAttack();
    void damageEnemysInPlayerRadius();
    void checkForDefeatedEnemies();
    inline void addVinicityToMap(Vicinity* newVinicity) { m_p_topMap_ = newVinicity; }
    inline void addEntityToMap(Entity* newEntity) { m_entityVector_.push_back(std::unique_ptr<Entity>(newEntity)); }
    inline void addEnemyToMap(Enemy* newEnemy) { m_enemyVector_.push_back(std::unique_ptr<Enemy>(newEnemy)); }
    inline std::vector<std::unique_ptr<Enemy>>* getEnemyVector() { return &m_enemyVector_; }
    inline std::vector<std::unique_ptr<Entity>>* getEntityVector() { return &m_entityVector_; }
    inline std::unique_ptr<Player>* getPlayer() { return &m_p_player_; }
};

