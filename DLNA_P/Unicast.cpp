#include "stdafx.h"
#include "Unicast.h"
#include "Device.h"
#include "DevList.h"

Unicast::Unicast()
{
    m_Group.sin_family = AF_INET;
    m_Group.sin_addr.s_addr = inet_addr("239.255.255.250");
    m_Group.sin_port = htons(1900);
    controller = NULL;
}

Unicast::~Unicast()
{
}

BOOL Unicast::CreateSocket(UINT TTL)
{
    if (!Create(0, SOCK_DGRAM, FD_READ | FD_OOB))
        return FALSE;
    if(!SetSockOpt(IP_MULTICAST_TTL, &TTL, sizeof(int), IPPROTO_IP))
    {
        Close();
        return FALSE;
    }
    int loopback = 0;
    if (!SetSockOpt(IP_MULTICAST_LOOP, &loopback, sizeof(int), IPPROTO_IP))
    {
        Close();
        return FALSE;
    }
    return TRUE;

}
BOOL Unicast::SendMsg(LPSTR devType)
{
    const CHAR MSG[] = "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 1\r\nST: %s\r\n\r\n";
    CHAR CompleteMSG[1024];
    sprintf_s(CompleteMSG, sizeof(CompleteMSG), MSG, devType);
    if (SendTo(CompleteMSG, strlen(CompleteMSG), (SOCKADDR *)&m_Group, sizeof(SOCKADDR)) == SOCKET_ERROR)
        return FALSE;
    CreateTimerQueueTimer(&m_searchTimer, NULL, Unicast::TimerProc, this, 3000, 0, 0);
    if (controller)
        controller->OnSearchStarted();
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
        if(controller) controller->OnHttpResponseReceived(buffer, FALSE, TRUE);
    }
	CAsyncSocket::OnReceive(nErrorCode); 
}

void Unicast::SetController(Controller *controller)
{
    this->controller = controller;
}

void CALLBACK Unicast::TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired)
{
    Unicast* obj = (Unicast*)lpParametar;
    obj->QueueTimerHandler();
}

void Unicast::QueueTimerHandler()
{
    if (DeleteTimerQueueTimer(NULL, m_searchTimer, NULL))
        m_searchTimer = NULL;
    if (controller)
        controller->OnSearchComplete();
}