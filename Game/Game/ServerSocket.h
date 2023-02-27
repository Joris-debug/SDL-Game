#pragma once
#include "Socket.h"
#include <Ws2tcpip.h> 

class ServerSocket
{
private:
  int			port;
  SOCKET		serverSocket;

public:
				ServerSocket	(int port);
				Socket* accept	(void);
  void			close			(void);
};