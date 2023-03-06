#include "GameServer.h"
#include "ServerSocket.h"
#include "World.h"
#include "Beetle.h"
#include "Player.h"
#include "GameHandler.h"

GameServer::GameServer(int port, World* m_p_world, GameHandler* m_p_gameHandler)
{
	m_p_serverSocket = new ServerSocket(port);
	this->m_p_currentWorld = m_p_world;
	this->m_p_gameHandler = m_p_gameHandler;
}

GameServer::~GameServer()
{
	delete m_p_serverSocket;
}

void GameServer::run()
{
	Socket* p_workSocket = m_p_serverSocket->accept();
	m_p_gameHandler->updateConnectionEstablished(true);
	bool* p_transmitNewFrame = m_p_gameHandler->getFrameTransmitted();

	while (m_threadIsRunning) {
		while (*p_transmitNewFrame)	//Wait for new frame
			Sleep(1);
		*p_transmitNewFrame = true;

		p_workSocket->write("OK");
		m_p_currentWorld->setServerLock(true); //stop the world from interfering with this thread iterating trough the vector
		int vectorSize = int(m_p_currentWorld->getEnemyVector()->size());
		std::cout << vectorSize << std::endl;
		p_workSocket->write(vectorSize);	//So the client knows how many enemies will be transmitted

		for (auto cursor : *m_p_currentWorld->getEnemyVector()) {
			p_workSocket->write(cursor->getEnemyId());							//Enemy identification Nr
			p_workSocket->write(static_cast<int>(cursor->getEnemyType()));		//Enemy Type (needed for the creation of the enemy)
			p_workSocket->write(round(cursor->getBounds()->x * 10.0f));
			p_workSocket->write(round(cursor->getBounds()->y * 10.0f));
			p_workSocket->write(static_cast<int>(cursor->getCurrentMode()));
			p_workSocket->write(cursor->getCurrentSprite());
		}
		Player* p_player = m_p_currentWorld->getPlayer();
		p_workSocket->write(round(p_player->getBounds()->x * 10.0f));
		p_workSocket->write(round(p_player->getBounds()->y * 10.0f));
		p_workSocket->write(static_cast<int>(p_player->getCurrentMode()));
		p_workSocket->write(p_player->getCurrentSprite());
		m_p_currentWorld->setServerLock(false);
	}
	m_p_serverSocket->close();
}
