#ifndef MULTICAST_H
#define MULTICAST_H
#include "Controller.h"

class Multicast:public CAsyncSocket
{
public:
    Multicast();
    ~Multicast();
    BOOL CreateSocket();
    virtual void OnReceive(int nErrorCode);
    void SetController(Controller *controller);
private:
    SOCKADDR_IN m_groupAddr;
    ip_mreq m_membership;
    Controller *controller;
};

#endif
