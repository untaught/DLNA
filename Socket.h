#include "MainH.h"

#ifndef SOCKET_H
#define SOCKET_H

class UnicastSocket
{
public:
    UnicastSocket(char ttl);
    ~UnicastSocket();
    LPSTR OnReceive(LPSTR databuf);

private:
    SOCKET SendSocket;

protected:
    in_addr LocalIP;
    sockaddr_in groupSock;
    WORD sockVersion;
    WSADATA wsaData;
};

#endif