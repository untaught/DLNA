#include "MainH.h"

#ifndef SOCKET_H
#define SOCKET_H

class UnicastSocket
{
public:
    UnicastSocket();
    ~UnicastSocket();
    int Create(char ttl); // returns 1 on success and 0 on failure
    int Send();// returns 1 on success and 0 on failure
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
