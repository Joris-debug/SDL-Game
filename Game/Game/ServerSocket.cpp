#include <iostream>
#include "ServerSocket.h"

ServerSocket::ServerSocket(int port)
{
  this->port = port;
  // WinSock-DLL einbinden
  WSADATA wsa;
  WSAStartup(MAKEWORD(2,0),&wsa);
  // Server-Socket erzeugen
  this->serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);
  if (this->serverSocket <= 0) 
  {
    std::cerr << "Error: Socket\n";
    return;
  }

  // Erzeuge die Socketadresse des Servers
  SOCKADDR_IN myAddr;
  memset( &myAddr, 0, sizeof(SOCKADDR_IN));
  myAddr.sin_family = AF_INET;
  myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myAddr.sin_port = htons((short)this->port);

  // Erzeuge die Bindung an die Serveradresse
  // (d.h. an einen bestimmten Port)
  if (bind(serverSocket, (SOCKADDR*)&myAddr, sizeof(SOCKADDR_IN)) == -1) 
  {
    closesocket(serverSocket);
    std::cerr << "Error: bind\n";
    return;
  }
  // Teile dem Socket mit, dass Verbindungswunsch
  // eines Clients entgegengenommen wird
  if (listen(serverSocket, 5) == -1) 
  {
    closesocket(serverSocket);
    std::cerr << "Error: listen\n";
    return;
  }
}

Socket* ServerSocket::accept()
{
  // Bearbeite die Verbindungswunsch von Clients
  // Der Aufruf von accept() blockiert solange,
  // bis ein Client Verbindung aufnimmt
  SOCKADDR_IN remoteAddr;
  int len = sizeof(SOCKADDR_IN);
  SOCKET clientSocket = ::accept(serverSocket, (SOCKADDR*)&remoteAddr, &len);
  if (clientSocket > 0) 
  {
    return new Socket(clientSocket);
  }
  return NULL;
}

void ServerSocket::close()
{
  closesocket(serverSocket);
}