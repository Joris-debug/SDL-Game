#pragma once
#include "WorkThread.h"

class World;
class GameHandler;
class ServerSocket;

class GameServer :
    public WorkThread
{
private:
    World* m_p_currentWorld;
    ServerSocket* m_p_serverSocket;
    GameHandler* m_p_gameHandler;
public:
    GameServer(World* m_p_world, GameHandler* m_p_gameHandler);
    ~GameServer();
    void run() override;
};

