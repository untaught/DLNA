#ifndef UNICAST_H
#define UNICAST_H
#include "Controller.h"

class Unicast: public CAsyncSocket
{
public:
    Unicast();
    ~Unicast();
    BOOL CreateSocket(UINT TTL);
    BOOL SendMsg(LPSTR Msg);
    virtual void OnReceive(int nErrorCode);
    void SetController(Controller *controller);
private:
    SOCKADDR_IN m_Group;
    Controller *controller;
};

#endif