#ifndef StreamServer_H
#define StreamServer_H

#include "Controller.h"
#include <atomic>

class StreamServer: public CAsyncSocket
{
public:
    StreamServer(LPSTR filepath, Controller *controller);
    ~StreamServer();
    StreamServer(const StreamServer &other);
    StreamServer & operator= (const StreamServer &other);
    void StartPartialStreamThread();
    void StartEntireContentStreamThread();
    BOOL SendHeadMsg();
    UINT GetFileSize();
    LPSTR GetFileTypeAndExtension(LPSTR TypeExtension);
    void PartialStream();
    void EntireStream();
    virtual void OnReceive(int nErrorCode);
    static UINT PartialStreamThreadFunction(LPVOID param);
    static UINT EntireStreamThreadFuntion(LPVOID param);
    void StopStream();
private:
    CWinThread *m_partial, *m_entire;
    std::atomic<bool> m_continue;
    FILE *m_file;
    LPSTR m_filepath;
    UINT m_bottomBoundary, m_upperBoundary;
    Controller *controller;
};

#endif