#include "stdafx.h"
#include "Multicast.h"

Multicast::Multicast()
{
    m_membership.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
    m_membership.imr_interface.s_addr = htons(INADDR_ANY);
}

Multicast::~Multicast()
{
    Close();
}

BOOL Multicast::CreateSocket()
{
    if(!Create(1900, SOCK_DGRAM, FD_READ))
        return FALSE;
    BOOL MultipleApps = TRUE;		
	if(!(SetSockOpt(SO_REUSEADDR, (void*)&MultipleApps, sizeof(BOOL), SOL_SOCKET)))
    {
        Close();
        return FALSE;
    }
    if(!SetSockOpt(IP_ADD_MEMBERSHIP,&m_membership,sizeof(ip_mreq),IPPROTO_IP))
    {
        Close();
        return FALSE;
    }
    return TRUE;
}

void Multicast::OnReceive(int nErrorCode)
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
            buffer[recv] = 0;
	}
	CAsyncSocket::OnReceive(nErrorCode); 
}

void Multicast::SetController(Controller *controller)
{
    this->controller = controller;
}