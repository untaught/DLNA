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
    void SetFilePath(LPSTR filepath){ strcpy_s(m_filepath, sizeof(m_filepath), filepath); };
    LPSTR GetFilepath(){ return m_filepath; };
    BOOL CreateSocket(UINT port = 65001);
    virtual void OnAccept(int nErrorCode);
    void SetController(Controller *controller);
    void ClearVector();
    std::vector<StreamServer *> vec;
private:
    CHAR m_filepath[MAX_PATH];
    Controller *controller;
    SOCKADDR_IN m_connectedSockAddr;
};

#endif