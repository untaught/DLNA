#include "stdafx.h"
#include "Unicast.h"
#include "Device.h"
#include "DevList.h"
#include "Controller.h"

Unicast::Unicast()
{
    m_Group.sin_family = AF_INET;
    m_Group.sin_addr.s_addr = inet_addr("239.255.255.250");
    m_Group.sin_port = htons(1900);
}

Unicast::~Unicast()
{
    Close();
}

BOOL Unicast::CreateSocket(UINT TTL)
{
    if(!Create(0, SOCK_DGRAM, FD_READ, 0))
        return FALSE;
    if(SetSockOpt(IP_MULTICAST_TTL, &TTL, sizeof(int), IPPROTO_IP) == 0)
    {
        Close();
        return FALSE;
    }
    int loopback = 0;
    if (SetSockOpt(IP_MULTICAST_LOOP, &loopback, sizeof(int), IPPROTO_IP) == 0)
    {
        Close();
        return FALSE;
    }
    return TRUE;

}
BOOL Unicast::SendMsg(LPSTR Msg)
{
    if(SendTo(Msg, strlen(Msg), (SOCKADDR *)&m_Group, sizeof(SOCKADDR)) == SOCKET_ERROR)
        {int asd = WSAGetLastError();
    return FALSE;}
    else
        return TRUE;
}

void Unicast::OnReceive(int nErrorCode)
{
	if (nErrorCode)
	{
		CAsyncSocket::OnReceive(nErrorCode);
		return;
	}
    CHAR buffer[700];
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
        if(controller) controller->OnHttpResponseReceived(buffer, FALSE);
    }
	CAsyncSocket::OnReceive(nErrorCode); 
}

void Unicast::SetController(Controller *controller)
{
    this->controller = controller;
}