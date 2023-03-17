#pragma once
#include "WorkThread.h"
#include <string>

class World;
class GameHandler;
class Socket;

class GameClient :
    public WorkThread
{
private:
    World* m_p_currentWorld;
    Socket* m_p_socket;
    GameHandler* m_p_gameHandler;
public:
    GameClient(std::string host, World* m_p_world, GameHandler* m_p_gameHandler);
    ~GameClient();
    void run() override;
};

