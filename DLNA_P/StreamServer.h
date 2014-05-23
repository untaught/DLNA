#ifndef StreamServer_H
#define StreamServer_H

#include "Controller.h"

class StreamServer: public CAsyncSocket
{
public:
    StreamServer(LPSTR filepath, Controller *controller);
    ~StreamServer();
    StreamServer(const StreamServer &other);
    StreamServer & operator= (const StreamServer &other);
    void SendMsg();
    void SendMsg1();
    BOOL SendHeadMsg();
    unsigned int GetFileSize();
    LPSTR GetFileTypeAndExtension(LPSTR TypeExtension);
    void Stream();
    void Stream1();
    virtual void OnReceive(int nErrorCode);
    virtual void OnClose(int nErrorCode);
    static UINT THREAD_FUNC(LPVOID param);
    static UINT THREAD_FUNC1(LPVOID param);
private:
    FILE *m_file;
    LPSTR m_filepath;
    unsigned int m_bottomBoundary, m_upperBoundary;
    Controller *controller;
};

#endif