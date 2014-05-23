#ifndef TCPServer_H
#define TCPServer_H
#include "Controller.h"
#include "StreamServer.h"
#include <vector>

class TCPServer: public CAsyncSocket
{
public:
    TCPServer();
    ~TCPServer();
    void SetPort(UINT port){ m_port = port; };
    UINT GetPort(){ return m_port; };
    void SetFilePath(LPSTR filepath){ strcpy_s(m_filepath, sizeof(m_filepath), filepath); };
    LPSTR GetFilepath(){ return m_filepath; };
    BOOL CreateSocket();
    virtual void OnAccept(int nErrorCode);
    void SetController(Controller *controller);
    void ClearVector();
    std::vector<StreamServer *> vec;
private:
    CHAR m_filepath[MAX_PATH];
    UINT m_port;
    Controller *controller;
    SOCKADDR_IN m_connectedSockAddr;
};

#endif