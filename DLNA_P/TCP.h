#ifndef TCP_H
#define TCP_H

class TCP:public CAsyncSocket
{
public:
    TCP();
    ~TCP();
    BOOL CreateSocket(LPSTR location);
    BOOL SendMsg(CHAR *path);
    virtual void OnReceive(int nErrorCode);
    virtual void OnConnect(int nErrorCode);
    virtual void OnClose(int nErrorCode);
    void SetController(Controller *controller);
private:
    Controller *controller;
    SOCKADDR_IN m_DevLoc;
    CHAR m_path[40];
};

#endif