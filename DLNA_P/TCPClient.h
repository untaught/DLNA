#ifndef TCPClient_H
#define TCPClient_H
#include "Controller.h"

enum Phase
{
    PHASE_GET_DESCRIPTION,
    PHASE_GET_PROTOCOL_INFO,
    PHASE_SET_AV_TRANSPORT,
    PHASE_SET_PLAY,
    PHASE_SET_STOP
};

class TCPClient: public CAsyncSocket
{
public:
    TCPClient();
    ~TCPClient();
    BOOL CreateSocket(LPSTR location, LPSTR ctrlURL = "");
    BOOL SendMsg();
    BOOL SendMsgSOAP();
    LPSTR GetLocalHostIP(LPSTR IP);
    virtual void OnReceive(int nErrorCode);
    virtual void OnConnect(int nErrorCode);
    void SetController(Controller *controller);
    int GetPhase() { return m_phase; };
    void SetPhase(Phase phase) { m_phase = phase; };
    void static CALLBACK TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired = TRUE);
    void QueueTimerHandler();
private:
    HANDLE m_connectionTimer;
    Phase m_phase;
    Controller *controller;
    SOCKADDR_IN m_DevLoc;
    CHAR m_path[MAX_PATH];
    CHAR m_url[MAX_PATH];
};

#endif