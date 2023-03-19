#pragma once
#include "Vicinity.h"
#include "Body.h"
#include "Entity.h"
#include <vector>
#include <iostream>
#include <random>

class Player;
class Enemy;
class TradingPost;
class Effect;
class PlayerTwo;

class World :
    public Vicinity
{
private:
    Vicinity* m_p_topMap;
    TradingPost* m_p_merchant;
    bool m_merchantIsActive;
    Player* m_p_player;
    PlayerTwo* m_p_playerTwo;
    bool m_triggerMerchantSpawnOnClient;
    std::vector<Enemy*> m_enemyVector;
    std::vector<Entity*> m_entityVector;
    std::mt19937* m_p_randomNumberEngine;
    bool m_serverLock;  //Enemies cant be deleted when this lock is set to true, this ensures thread safety
    bool m_transmitEnemyHitSound;
    bool m_transmitPlayerHitSound;
public:
    World(SDL_Surface* surface, SDL_FRect m_bounds_, SDL_Renderer* renderer, std::mt19937* m_p_randomNumberEngine_, Effect* m_p_spawnEffect_);
    ~World();
    walkingVector checkPlayerMove(float x, float y, float deltaTime);
    void moveWorld(float x, float y, float deltaTime);
    void renderWorld(SDL_Renderer* renderer);
    void triggerPlayerAttack();
    void damageEnemysInPlayerRadius();
    void damageEnemysInPlayerTwoRadius();
    void checkIfPlayerHit();
    void checkIfPlayerTwoHit();
    void checkForDefeatedEnemies();
    void makeMerchantAppear();
    bool trySpawningMerchantClose();
    bool trySpawningMerchantFar();
    bool talkToMerchant(); // Returns true if the player was able to talk to the merchant
    void sendMerchantAway();
    bool checkIfMerchantDespawned();
    SDL_FPoint getRandomCoordinate();
    int getRandomNumber(int rangeBegin, int rangeEnde); //Used to generate random coordinate
    bool checkIfEnemyExists(int enemyId);
    Enemy* getEnemyById(int enemyId);
    void deleteNotExistingVirtualEnemies(std::vector<int> existingEnemies);
    SDL_FPoint getCloserPoint(SDL_FPoint start, SDL_FPoint end1, SDL_FPoint end2);
    inline bool* getServerLock() { return &m_serverLock; }
    inline TradingPost* getMerchant() { return m_p_merchant; }
    inline void addVinicityToMap(Vicinity* newVinicity) { m_p_topMap = newVinicity; }
    inline void addEntityToMap(Entity* newEntity) { m_entityVector.push_back(newEntity); }
    inline void addEnemyToMap(Enemy* newEnemy) { m_enemyVector.push_back(newEnemy); }
    inline bool getMerchantIsActive() { return m_merchantIsActive; }
    inline std::vector<Enemy*>* getEnemyVector() { return &m_enemyVector; }
    inline std::vector<Entity*>* getEntityVector() { return &m_entityVector; }
    inline Player* getPlayer() { return m_p_player; }
    inline PlayerTwo* getPlayerTwo() { return m_p_playerTwo; }
    inline void setPlayerTwo(PlayerTwo* m_p_playerTwo) { this->m_p_playerTwo = m_p_playerTwo; }
    inline bool getTriggerMerchantSpawnOnClient() { return (m_triggerMerchantSpawnOnClient) ? !(m_triggerMerchantSpawnOnClient = 0) : false; }
    inline bool getTransmitEnemyHitSound() { return (m_transmitEnemyHitSound) ? !(m_transmitEnemyHitSound = 0) : false; }
    inline bool getTransmitPlayerHitSound() { return (m_transmitPlayerHitSound) ? !(m_transmitPlayerHitSound = 0) : false; }
};

