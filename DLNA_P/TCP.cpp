#include "stdafx.h"
#include "Controller.h"
#include "TCP.h"

TCP::TCP()
{
}

TCP::~TCP()
{
    Close();
}

BOOL TCP::CreateSocket(LPSTR location)
{
    //get local ip
    CHAR localhost[50];
    char *myip;
    gethostname(localhost, sizeof(localhost));
    struct hostent *host = gethostbyname(localhost);
    myip = inet_ntoa(*(struct in_addr*)*host->h_addr_list);

    //take IP
    CHAR ip[16];
    int i=0;
    location = strstr(location, "/") + 2;
	while (*location && *location != ':')
        ip[i++] = *location++;
	ip[i++]=0;
    
    //take port
    CHAR port[8];
    location = strstr(location, ":") + 1;
    i=0;
    while (*location && *location != '/')
        port[i++] = *location++;
    port[i++] = 0;

    //take path
    i=0;
    if (*location)
    {
        while (*location)
            m_path[i++] = *location++;
        m_path[i++] = 0;
    }
    else 
    {
        m_path[0] = '/';
        m_path[1] = 0;
    }

    m_DevLoc.sin_family = AF_INET;
    m_DevLoc.sin_port = htons(atoi(port));
    m_DevLoc.sin_addr.s_addr = inet_addr(ip);

    if(!Create())
        return FALSE;
    BOOL keepAlive=TRUE;
    if(!SetSockOpt(SO_KEEPALIVE, &keepAlive, sizeof(BOOL), SOL_SOCKET))
    {
        Close();
        return FALSE;
    }
    Connect((SOCKADDR *)&m_DevLoc,sizeof(SOCKADDR));
    return TRUE;
}

void TCP::OnConnect(int nErrorCode)
{
    if(nErrorCode)
    {
        CAsyncSocket::OnConnect(nErrorCode);
        return;
    }
    SendMsg(m_path);
    CAsyncSocket::OnConnect(nErrorCode);
}

BOOL TCP::SendMsg(CHAR *path)
{
    const CHAR MSG[] = "GET %s HTTP/1.1\r\nHOST: %s:%d\r\n\r\n";
    CHAR CompleteMSG[300];
    sprintf_s(CompleteMSG, sizeof(CompleteMSG), MSG, path, inet_ntoa(m_DevLoc.sin_addr), ntohs(m_DevLoc.sin_port));
    if(!Send(CompleteMSG, strlen(CompleteMSG)))
        return FALSE;

    return TRUE;
}

void TCP::OnClose(int nErrorCode)
{
    Close();
}

void TCP::OnReceive(int nErrorCode)
{
   if (nErrorCode)
	{
		CAsyncSocket::OnReceive(nErrorCode);
		return;
	}
    CHAR buffer[7000];
	int recv = Receive(buffer, sizeof(buffer));
 	if (recv == SOCKET_ERROR)
	{
		CAsyncSocket::OnReceive(nErrorCode);
		return;
	}
	else
	{
		if (recv>0) 
            buffer[recv]=0;
        if(controller) controller->OnHttpResponseReceived(buffer, TRUE);
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

void TCP::SetController(Controller *controller)
{
    this->controller = controller;
}