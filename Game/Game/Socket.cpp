#include "Socket.h"

Socket::Socket(std::string host, int port)
{
  this->port = port;
  this->host = host;
  // WinSock-DLL einbinden
  WSADATA m_wsa;
  WSAStartup(MAKEWORD(2,0), &m_wsa);
  // CSocket erzeugen
  this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
}

Socket::Socket(SOCKET socket)
{
   this->socket = socket;
}

bool Socket::connect()
{
  SOCKADDR_IN addr;
  memset( &addr, 0, sizeof( SOCKADDR_IN ));

  addr.sin_family = AF_INET;
  addr.sin_port = htons((short)this->port);
  inet_pton(addr.sin_family,this->host.c_str(),&addr.sin_addr);

  // Verbindung herstellen
  int rc = ::connect(socket, (SOCKADDR*) &addr, sizeof( SOCKADDR ));

  return rc != SOCKET_ERROR;
}

int Socket::dataAvailable()
{
	//Die dataAvailable-Funktuion wurde mit dem Select-Befehl so modifiziert, 
	//dass sie nicht mehr blockierend ist. Sie wartet nur noch 1 Sekunde!

    /* //Ursprünglicher Code
	const int len = 1024;
	char b[len];
	return recv(socket, b, len, MSG_PEEK);
	*/

	fd_set set;
	FD_ZERO(&set);
	FD_SET(socket, &set);
	timeval timeout;
	timeout.tv_sec = long(1); //Wartezeit von einer halben Sekunde
	return select(0,&set,NULL,NULL,&timeout);
}

void Socket::write(int b)
{
	//Im Zuge dessen, dass die write-Funktion vortäuschte ein Integer zu übertragen,
	//doch letztlich nur 8 Bits (1 Byte/Char) übertrug, wurde die Funktion
	//so umgeändert, dass ein Integer (32 Bits) übertragen wird

	/* //Ursprünglicher Code
	char tmp[1];
	tmp[0] = (char)b;
	send(socket, tmp, 1, 0);
	*/

	char tmp[4];

	//Die jeweiligen Umrechnung, damit ein Char jeweils 4 bits enthält/trägt
	tmp[0] = unsigned char (b & 0xFF);
	tmp[1] = unsigned char((b & 0xFF00) / 256);
	tmp[2] = unsigned char((b & 0xFF0000) / 65536);
	tmp[3] = unsigned char((b & 0xFF000000) / 16777216);
	send(socket, tmp, 4, 0);
}

void Socket::write(char*b, int len)
{
	strcat_s(b,strlen(b),"\n");
	send(socket, b, len, 0);
}

void Socket::write(std::string s)
{
	s+= '\n';
	send(socket, s.c_str(), s.length(), 0);
}

int Socket::read()
{
	//Diese Funktion musste ebenfalls angepasst werden, damit sie einen Integer empfängt/ausliest,
	//um mit der oben modifizierten write-Funtkion kompatibel zu sein

	/* //Ursprünglicher Code
	char tmp[1];
	recv(socket, tmp, 1, 0);
	return tmp[0];
	*/

	int returnInteger;
	char tmp[4];

	recv(socket, tmp, 4, 0);
	returnInteger = unsigned char(tmp[0]) + unsigned char(tmp[1]) * 256 + unsigned char(tmp[2]) * 65536 + unsigned char(tmp[3]) * 16777216;

	return returnInteger;
}

int Socket::read(char* b, int len)
{
  return recv(socket, b, len, 0);
}

std::string Socket::readLine()
{
  char buffIn[256];
  int pos = -1;
  do 
  {
    pos++;
    recv(socket, buffIn+pos, 1, 0);
  } while (buffIn[pos] != '\n');

  buffIn[pos] = '\0';  // echo-std::string ist mit '\n' abgechlossen
  return std::string(buffIn);
}

void Socket::close()
{
  closesocket(socket);
}