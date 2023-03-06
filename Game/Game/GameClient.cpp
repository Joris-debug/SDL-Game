#include "GameClient.h"
#include "Socket.h"
#include "World.h"
#include "Player.h"
#include "GameHandler.h"
#include "VirtualEnemy.h"
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
	while (m_threadIsRunning) {
		std::cout << m_p_socket->readLine() << std::endl;
		m_p_currentWorld->setServerLock(true); //stop the world from interfering with this thread iterating trough the vector

		int vectorSize = m_p_socket->read();	//So the client knows how many enemies will be transmitted
		std::cout << vectorSize << std::endl;

		for (int i = 0; i < vectorSize; i++) {
			int enemyId = m_p_socket->read();
			Uint8 enemyType = m_p_socket->read();
			std::cout << enemyId << std::endl;
			SDL_Point enemyPos;
			enemyPos.x = m_p_socket->read();
			enemyPos.y = m_p_socket->read();
			Uint8 EnemyMode = m_p_socket->read();
			short currentSprite = m_p_socket->read();
			if (!m_p_currentWorld->checkIfEnemyExists(enemyId)) {
				m_p_gameHandler->createNewVirtualEnemy(enemyId, enemyType, enemyPos);
			}
		}
		std::cout << "Player:\n";
		std::cout << m_p_socket->read() << std::endl;
		std::cout << m_p_socket->read() << std::endl;
		std::cout << m_p_socket->read() << std::endl;
		std::cout << m_p_socket->read() << std::endl;
		m_p_currentWorld->setServerLock(false);
	}
	m_p_socket->close();
}
