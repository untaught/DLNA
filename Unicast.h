#include "MainH.h"

#ifndef UNICAST_H
#define UNICAST_H

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
    in_addr LocalIP;
    sockaddr_in groupSock;

protected:
    WORD sockVersion;
    WSADATA wsaData;
};

#endif
