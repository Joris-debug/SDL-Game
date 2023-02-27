#include "GameServer.h"
#include "ServerSocket.h"
#include "World.h"
#include "Enemy.h"
#include "Player.h"
#include "GameHandler.h"

GameServer::GameServer(int port, World* m_p_world, GameHandler* m_p_gameHandler)
{
	m_p_serverSocket = new ServerSocket(port);
	this->m_p_world = m_p_world;
	this->m_p_gameHandler = m_p_gameHandler;
}

GameServer::~GameServer()
{
	delete m_p_serverSocket;
}

void GameServer::run()
{
	Socket* p_workSocket = m_p_serverSocket->accept();
	bool* p_transmitNewFrame = m_p_gameHandler->getFrameTransmitted();
	while (m_threadIsRunning) {
		while (*p_transmitNewFrame);	//Wait for new frame
		*p_transmitNewFrame = true;

		p_workSocket->write("[ENEMIES]\n");
		for (auto cursor : *m_p_world->getEnemyVector()) {
			p_workSocket->write(cursor->getBounds()->x);
			p_workSocket->write(cursor->getBounds()->y);
		}
		p_workSocket->write("[PLAYER]\n");
		m_p_world->getPlayer()->getBounds()->x;
		m_p_world->getPlayer()->getBounds()->y;
	}
	m_p_serverSocket->close();
}
