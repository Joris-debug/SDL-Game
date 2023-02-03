#pragma once
#include "Vicinity.h"
#include "Body.h"
#include "Entity.h"
#include <vector>
#include <iostream>
#include <random>
class Interface;
class Player;
class Enemy;
class TradingPost;
class Effect;

class World :
    public Vicinity
{
private:
    Vicinity* m_p_topMap_;
    TradingPost* m_p_merchant_;
    bool m_merchantIsActive_;
    Player* m_p_player_;
    std::vector<std::unique_ptr<Enemy>> m_enemyVector_;
    std::vector<Entity*> m_entityVector_;
    std::mt19937* m_p_randomNumberEngine_;
public:
    World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer, std::mt19937* m_p_randomNumberEngine_, Effect* m_p_spawnEffect_);
    ~World();
    walkingVector checkPlayerMove(float x, float y, float deltaTime);
    void moveWorld(float x, float y, float deltaTime);
    void renderWorld(SDL_Renderer* renderer);
    void triggerPlayerAttack();
    void damageEnemysInPlayerRadius();
    void checkIfPlayerHit();
    void checkForDefeatedEnemies();
    void makeMerchantAppear();
    bool trySpawningMerchantClose();
    bool trySpawningMerchantFar();
    void sendMerchantAway();
    bool checkIfMerchantDespawned();
    SDL_FPoint getRandomCoordinate();
    int getRandomNumber(int rangeBegin, int rangeEnde); //Used to generate random coordinate
    inline void addVinicityToMap(Vicinity* newVinicity) { m_p_topMap_ = newVinicity; }
    inline void addEntityToMap(Entity* newEntity) { m_entityVector_.push_back(newEntity); }
    inline void addEnemyToMap(Enemy* newEnemy) { m_enemyVector_.push_back(std::unique_ptr<Enemy>(newEnemy)); }
    inline bool getMerchantIsActive() { return m_merchantIsActive_; }
    inline std::vector<std::unique_ptr<Enemy>>* getEnemyVector() { return &m_enemyVector_; }
    inline std::vector<Entity*>* getEntityVector() { return &m_entityVector_; }
    inline Player* getPlayer() { return m_p_player_; }
};

