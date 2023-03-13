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
	std::cout << "Delete Client start\n";
	m_threadStatus = MultiplayerStatus::gameEnded;	//To ensure the app doesnt wait forever
	m_p_thread->join();
	delete m_p_socket;
	std::cout << "Delete Client end\n";
}

void GameClient::run()
{
	if (!m_p_socket->connect()) {
		std::cout << "Error";
		return;
	}

	m_p_gameHandler->updateConnectionEstablished(true);
	MultiplayerStatus serverStatus;
	std::vector<int> existingEnemies;
	Player* p_player = m_p_currentWorld->getPlayer();
	PlayerTwo* p_playerTwo = m_p_currentWorld->getPlayerTwo();

	while (true) {
		int vectorSize = m_p_socket->read();	//So the client knows how many enemies will be transmitted
		//std::cout << vectorSize << std::endl;
		existingEnemies.clear();			//I dont know which enemies exist at the moment

		bool* p_serverLock = m_p_currentWorld->getServerLock();
		while (*p_serverLock);
		*p_serverLock = true; //stop the world from interfering with this thread iterating trough the vector
		SDL_FRect* p_mapBounds = m_p_currentWorld->getBounds();
		
		for (int i = 0; i < vectorSize; i++) {

			int enemyId = m_p_socket->read();
			existingEnemies.push_back(enemyId);
			Uint8 enemyType = m_p_socket->read();

			SDL_FPoint enemyPos;
			enemyPos.x = p_mapBounds->x - float(m_p_socket->read()) / 10.0f;
			enemyPos.y = p_mapBounds->y - float(m_p_socket->read()) / 10.0f;

			Uint8 enemyMode = m_p_socket->read();
			short currentSprite = m_p_socket->read();
			int textureCoordsY = m_p_socket->read();

			if (!m_p_currentWorld->checkIfEnemyExists(enemyId))
				m_p_gameHandler->createNewVirtualEnemy(enemyId, enemyType, enemyPos);	//create new enemy
			//---------------------------------------------------------------------------------------------------- Update enemy data
			Enemy* p_enemy = m_p_currentWorld->getEnemyById(enemyId);	//find the enemy
			p_enemy->getTextureCoords()->y = textureCoordsY;
			p_enemy->setAnimation(enemyMode, currentSprite);
			p_enemy->teleportEnemy(enemyPos);
		}
		m_p_currentWorld->deleteNotExistingVirtualEnemies(existingEnemies);

		SDL_FPoint playerPos;
		playerPos.x = p_mapBounds->x - float(m_p_socket->read()) / 10.0f;
		playerPos.y = p_mapBounds->y - float(m_p_socket->read()) / 10.0f;

		Uint8 playerMode = m_p_socket->read();
		short currentSprite = m_p_socket->read();
		bool currentDirection = m_p_socket->read();

		p_playerTwo->setCurrentDirection(currentDirection);
		p_playerTwo->setAnimation(playerMode, currentSprite);
		p_playerTwo->teleportPlayerTwo(playerPos);
		*p_serverLock = true;

		m_p_gameHandler->updateWaveCounter(m_p_socket->read());
		if (m_p_socket->read())			//This transmitted flag tells the client that its been hit
			p_player->damageBody(1);
		//------------------------------------------------------------------------------------------------------- Client will now send its own player data
		m_p_socket->write(round((p_mapBounds->x - p_player->getBounds()->x) * 10.0f));
		m_p_socket->write(round((p_mapBounds->y - p_player->getBounds()->y) * 10.0f));
		m_p_socket->write(static_cast<int>(p_player->getCurrentMode()));
		m_p_socket->write(p_player->getCurrentSprite());
		m_p_socket->write(p_player->getCurrentDirection());
		//------------------------------------------------------------------------------------------------------- Client will now receive info about the game

		m_p_socket->write(p_player->getNewAttackTriggered());
		
		if (m_p_socket->read()) //If the merchant on the server is active
			m_p_currentWorld->makeMerchantAppear();
		m_p_socket->write(m_p_currentWorld->getMerchantIsActive());

		m_p_socket->write(static_cast<int>(m_threadStatus));
		serverStatus = static_cast<MultiplayerStatus>(m_p_socket->read());

		if (serverStatus != MultiplayerStatus::isRunning || m_threadStatus != MultiplayerStatus::isRunning) {
			std::cout << "Client ended\n";
			break;
		}
	}

	if (serverStatus == MultiplayerStatus::gameOver)	//If the server dies, the client dies also
		p_player->damageBody(p_player->getCurrentLives());
	else if (serverStatus == MultiplayerStatus::gameEnded)	//If the server exits the game, the client will also
		m_p_gameHandler->setGameState(GameStates::hasEnded);

	m_p_socket->close();
}
