#include "MainH.h"
#include "Unicast.h"

#ifndef MULTICAST_H
#define MULTICAST_H

class MulticastSocket: public UnicastSocket
{
public:
    MulticastSocket();
    ~MulticastSocket();
    int Create();
    LPTSTR OnRecieve(LPSTR databuf);

private:
    SOCKET RecvSocket;
    SOCKADDR_IN localSock;
    struct ip_mreq group;
};

#endif
