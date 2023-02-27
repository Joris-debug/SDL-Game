#pragma comment(lib,"ws2_32.lib")
#pragma once
#include <Ws2tcpip.h>
#include <string>

class Socket
{
private:

  std::string	host;
  int			port;
  SOCKET		socket;

public:

				Socket			(std::string host, int port);
				Socket			(SOCKET socket);  // wird zur Socketerzeugung beim CServerSocket benoetigt
  bool			connect			(void);
  int			dataAvailable	(void);
  void			write			(int b);
  void			write			(char* b, int len);
  void			write			(std::string s);
  int			read			(void);
  int			read			(char* b, int len);
  std::string	readLine		(void);
  void			close			(void);
};