#include "stdafx.h"
#include "TCPServer.h"

TCPServer::TCPServer()
{
    controller = NULL;
}

TCPServer::~TCPServer()
{
    ClearVector();
}

BOOL TCPServer::CreateSocket(UINT port)
{
    if (!Create(port))
        return FALSE;
    BOOL keepAlive = TRUE;
    if(!SetSockOpt(SO_KEEPALIVE, (void*)&keepAlive, sizeof(BOOL), SOL_SOCKET))
    {
        Close();
        return FALSE;
    }
    if(!Listen(1))
        return FALSE;
    return TRUE;
}

void TCPServer::OnAccept(int nErrorCode)
{
    if(nErrorCode)
    {
        CAsyncSocket::OnAccept(nErrorCode);
        return;
    }
    int SAsize = sizeof(SOCKADDR);
    if (vec.size() > 0)
        vec.back()->StopStream();
    vec.push_back(new StreamServer(m_filepath, controller));
    if(Accept(*vec.back(), (SOCKADDR *)&m_connectedSockAddr, &SAsize) == SOCKET_ERROR)
    {
        CAsyncSocket::OnAccept(nErrorCode);
        return;
    }
    CAsyncSocket::OnAccept(nErrorCode);
}

void TCPServer::ClearVector()
{
    for (std::vector<StreamServer *>::iterator i = vec.begin(); i != vec.end(); ++i)
        delete *i;
    vec.clear();
    std::vector<StreamServer *>(vec).swap(vec);
}

void TCPServer::SetController(Controller *controller)
{
    this->controller = controller;
}
