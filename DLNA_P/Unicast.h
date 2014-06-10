#ifndef UNICAST_H
#define UNICAST_H
#include "Controller.h"

class Unicast: public CAsyncSocket
{
public:
    Unicast();
    ~Unicast();
    BOOL CreateSocket(UINT TTL = 32);
    BOOL SendMsg(LPSTR devType);
    virtual void OnReceive(int nErrorCode);
    void SetController(Controller *controller);
    void static CALLBACK TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired = TRUE);
    void QueueTimerHandler();
private:
    HANDLE m_searchTimer;
    SOCKADDR_IN m_Group;
    Controller *controller;
};

#endif