#include "GameClient.h"
#include "Socket.h"
#include "World.h"
#include "Player.h"
#include "GameHandler.h"
#include "VirtualEnemy.h"
#include "PlayerTwo.h"
#include <iostream>

GameClient::GameClient(int port, World* m_p_world, GameHandler* m_p_gameHandler)
{
	m_p_socket = new Socket("127.0.0.1", port);
	this->m_p_currentWorld = m_p_world;
	this->m_p_gameHandler = m_p_gameHandler;
}

GameClient::~GameClient()
{
	delete m_p_socket;
}

void GameClient::run()
{
	if (!m_p_socket->connect()) {
		std::cout << "Error";
		return;
	}

	m_p_gameHandler->updateConnectionEstablished(true);
	std::vector<int> existingEnemies;

	while (m_threadIsRunning) {
		std::cout << m_p_socket->readLine() << std::endl;

		int vectorSize = m_p_socket->read();	//So the client knows how many enemies will be transmitted
		std::cout << vectorSize << std::endl;
		existingEnemies.clear();			//I dont know which enemies exist at the moment

		bool* p_serverLock = m_p_currentWorld->getServerLock();
		while (*p_serverLock);
		*p_serverLock = true; //stop the world from interfering with this thread iterating trough the vector
		SDL_FRect* p_mapBounds = m_p_currentWorld->getBounds();
		
		for (int i = 0; i < vectorSize; i++) {
			int enemyId = m_p_socket->read();
			Uint8 enemyType = m_p_socket->read();
			existingEnemies.push_back(enemyId);
			SDL_FPoint enemyPos;
			enemyPos.x = p_mapBounds->x - float(m_p_socket->read()) / 10.0f;
			enemyPos.y = p_mapBounds->y - float(m_p_socket->read()) / 10.0f;
			Uint8 enemyMode = m_p_socket->read();
			short currentSprite = m_p_socket->read();
			if (!m_p_currentWorld->checkIfEnemyExists(enemyId))
				m_p_gameHandler->createNewVirtualEnemy(enemyId, enemyType, enemyPos);	//create new enemy
			Enemy* p_enemy = m_p_currentWorld->getEnemyById(enemyId);	//find the enemy
			p_enemy->setAnimation(enemyMode, currentSprite);
			p_enemy->teleportEnemy(enemyPos);
		}
		m_p_currentWorld->deleteNotExistingVirtualEnemies(existingEnemies);
		std::cout << "Player:\n";
		SDL_FPoint playerPos;
		playerPos.x = p_mapBounds->x - float(m_p_socket->read()) / 10.0f;
		playerPos.y = p_mapBounds->y - float(m_p_socket->read()) / 10.0f;
		Uint8 playerMode = m_p_socket->read();
		short currentSprite = m_p_socket->read();
		PlayerTwo* p_playerTwo = m_p_currentWorld->getPlayerTwo();
		p_playerTwo->setAnimation(playerMode, currentSprite);
		p_playerTwo->teleportPlayerTwo(playerPos);
		*p_serverLock = true;
	}
	m_p_socket->close();
}
