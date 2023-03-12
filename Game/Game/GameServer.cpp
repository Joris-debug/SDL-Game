#include "GameServer.h"
#include "ServerSocket.h"
#include "World.h"
#include "Beetle.h"
#include "Player.h"
#include "PlayerTwo.h"
#include "GameHandler.h"

GameServer::GameServer(int port, World* m_p_world, GameHandler* m_p_gameHandler)
{
	m_p_serverSocket = new ServerSocket(port);
	this->m_p_currentWorld = m_p_world;
	this->m_p_gameHandler = m_p_gameHandler;
}

GameServer::~GameServer()
{
	std::cout << "Delete Server start\n";
	m_threadStatus = MultiplayerStatus::gameEnded;	//To ensure the app doesnt wait forever
	m_p_thread->join();
	delete m_p_serverSocket;
	std::cout << "Delete Server end\n";
}

void GameServer::run()
{
	Socket* p_workSocket = m_p_serverSocket->accept();
	m_p_gameHandler->updateConnectionEstablished(true);
	bool* p_transmitNewFrame = m_p_gameHandler->getFrameTransmitted();

	MultiplayerStatus clientStatus;
	Player* p_player = m_p_currentWorld->getPlayer();
	PlayerTwo* p_playerTwo = m_p_currentWorld->getPlayerTwo();

	while (true) {

		while (*p_transmitNewFrame)	//Wait for new frame
			Sleep(1);
		*p_transmitNewFrame = true;

		bool* p_serverLock = m_p_currentWorld->getServerLock();
		while (*p_serverLock);
		*p_serverLock = true; //stop the world from interfering with this thread iterating trough the vector

		int vectorSize = int(m_p_currentWorld->getEnemyVector()->size());
		//std::cout << vectorSize << std::endl;
		p_workSocket->write(vectorSize);	//So the client knows how many enemies will be transmitted
		SDL_FRect* p_mapBounds = m_p_currentWorld->getBounds();

		for (auto cursor : *m_p_currentWorld->getEnemyVector()) {
			p_workSocket->write(cursor->getEnemyId());							//Enemy identification Nr
			p_workSocket->write(static_cast<int>(cursor->getEnemyType()));		//Enemy Type (needed for the creation of the enemy) 
			p_workSocket->write(round((p_mapBounds->x - cursor->getBounds()->x) * 10.0f)); //Enemy position relative to the map will be transmitted
			p_workSocket->write(round((p_mapBounds->y - cursor->getBounds()->y) * 10.0f));
			p_workSocket->write(static_cast<int>(cursor->getCurrentMode()));			//These 3 variables are needed for animation
			p_workSocket->write(cursor->getCurrentSprite());
			p_workSocket->write(cursor->getTextureCoords()->y);
		}

		p_workSocket->write(round((p_mapBounds->x - p_player->getBounds()->x) * 10.0f));
		p_workSocket->write(round((p_mapBounds->y - p_player->getBounds()->y) * 10.0f));
		p_workSocket->write(static_cast<int>(p_player->getCurrentMode()));
		p_workSocket->write(p_player->getCurrentSprite());
		p_workSocket->write(p_player->getCurrentDirection());

		p_workSocket->write(m_p_gameHandler->getWaveCounter());
		p_workSocket->write(p_playerTwo->getHitDetected());
		//------------------------------------------------------------------------------------------------ Server will now receive client player data
		SDL_FPoint playerPos;
		playerPos.x = p_mapBounds->x - float(p_workSocket->read()) / 10.0f;
		playerPos.y = p_mapBounds->y - float(p_workSocket->read()) / 10.0f;

		Uint8 playerMode = p_workSocket->read();
		short currentSprite = p_workSocket->read();
		bool currentDirection = p_workSocket->read();

		p_playerTwo->setCurrentDirection(currentDirection);
		p_playerTwo->setAnimation(playerMode, currentSprite);
		p_playerTwo->teleportPlayerTwo(playerPos);
		*p_serverLock = false;
		//------------------------------------------------------------------------------------------------ Client sends flag if a new attack started
		if(p_workSocket->read())
			p_playerTwo->triggerNewAttack();

		clientStatus = static_cast<MultiplayerStatus>(p_workSocket->read());
		p_workSocket->write(static_cast<int>(m_threadStatus));

		if (clientStatus != MultiplayerStatus::isRunning || m_threadStatus != MultiplayerStatus::isRunning) {
			std::cout << "Server ended\n";
			break;
		}
	}

	if (clientStatus == MultiplayerStatus::gameOver)	//If the client dies, the server dies also
		p_player->damageBody(p_player->getCurrentLives());
	else if (clientStatus == MultiplayerStatus::gameEnded)	//If the client exits the game, the server will also
		m_p_gameHandler->setGameState(GameStates::hasEnded);

	m_p_serverSocket->close();
}
