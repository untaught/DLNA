#include "stdafx.h"
#include "TCPClient.h"

TCPClient::TCPClient()
{
    controller = NULL;
    m_phase = PHASE_GET_DESCRIPTION;
}

TCPClient::~TCPClient()
{
}

BOOL TCPClient::CreateSocket(LPSTR location, LPSTR ctrlURL)
{
    if (!strlen(location) || (m_phase != PHASE_GET_DESCRIPTION && !strlen(ctrlURL)))
    {
        controller->OnBadDeviceLocation();
        return FALSE;
    }
    //set control URL
    if (ctrlURL)
        strcpy_s(m_url, sizeof(m_url), ctrlURL);

    //take IP
    CHAR ip[16];
    int i=0;
    LPSTR ipStartPos = strchr(location, '/') + 2;
    while (*ipStartPos && *ipStartPos != ':')
        ip[i++] = *ipStartPos++;
    ip[i++]=0;
    
    //take port
    CHAR port[8];
    ipStartPos = strchr(ipStartPos, ':') + 1;
    i=0;
    while (*ipStartPos && *ipStartPos != '/')
        port[i++] = *ipStartPos++;
    port[i++] = 0;

    //take path
    i=0;
    if (*ipStartPos)
    {
        while (*ipStartPos)
            m_path[i++] = *ipStartPos++;
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
    m_hSocket = INVALID_SOCKET;
    if(!Create())
        return FALSE;
    Connect((SOCKADDR *)&m_DevLoc, sizeof(SOCKADDR));
    CreateTimerQueueTimer(&m_connectionTimer, NULL, TCPClient::TimerProc, this, 5000, 0, 0);
    return TRUE;
}

void TCPClient::OnConnect(int nErrorCode)
{
    if(nErrorCode)
    {
        CAsyncSocket::OnConnect(nErrorCode);
        return;
    }
    DeleteTimerQueueTimer(NULL, m_connectionTimer, NULL);
    switch (m_phase)
    {
        case PHASE_GET_DESCRIPTION:
            SendMsg();
            break;
        case PHASE_GET_PROTOCOL_INFO:
        case PHASE_SET_AV_TRANSPORT:
        case PHASE_SET_PLAY:
        case PHASE_SET_STOP:
            SendMsgSOAP();
            break;
    }
    CAsyncSocket::OnConnect(nErrorCode);
}

BOOL TCPClient::SendMsg()
{
    const CHAR MSG[] = "GET %s HTTP/1.1\r\nCache-Control: no-cache\r\nPragma: no-cache\r\nAccept: text/xml\r\nHost: %s:%d\r\n\r\n";
    CHAR CompleteMSG[250];
    sprintf_s(CompleteMSG, sizeof(CompleteMSG), MSG, m_path, inet_ntoa(m_DevLoc.sin_addr), ntohs(m_DevLoc.sin_port));
    if(!Send(CompleteMSG, strlen(CompleteMSG)))
        return FALSE;
    return TRUE;
}

BOOL TCPClient::SendMsgSOAP()
{
    CHAR localIP[16];
    CHAR CompleteMSG[1800] = {0};
    
    const CHAR msgConnMgr[] = "POST %s HTTP/1.1\r\nHost: %s:%d\r\nConnection: close\r\nContent-Length: 286\r\nContent-Type: text/xml; charset=\"utf-8\"\r\nSOAPAction: \"urn:schemas-upnp-org:service:ConnectionManager:1#GetProtocolInfo\"\r\n\r\n<?xml version=\"1.0\"?>\r\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"\r\ns:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n<s:Body>\r\n<u:GetProtocolInfo xmlns:u=\"urn:schemas-upnp-org:service:ConnectionManager:1\">\r\n</u:GetProtocolInfo>\r\n</s:Body>\r\n</s:Envelope>";
    
    const CHAR msgAVTransSet_Head[] = "POST %s HTTP/1.1\r\nCache-Control: no-cache\r\nConnection: Close\r\nPragma: no-cache\r\nHost: %s:%d\r\nContent-Length: %d\r\nContent-Type: text/xml; charset=\"utf-8\"\r\nSOAPAction: \"urn:schemas-upnp-org:service:AVTransport:1#SetAVTransportURI\"\r\n\r\n%s";
    CHAR AVTransSet_Body[] = "<?xml version=\"1.0\"?>\r\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n<s:Body>\r\n<u:SetAVTransportURI xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\">\r\n<InstanceID>0</InstanceID>\r\n<CurrentURI>http://%s:65001/</CurrentURI>\r\n<CurrentURIMetaData></CurrentURIMetaData>\r\n</u:SetAVTransportURI>\r\n</s:Body>\r\n</s:Envelope>";
    CHAR FullAVTransSet_Body[430];

    const CHAR msgAVTransPlay[] = "POST %s HTTP/1.1\r\nCache-Control: no-cache\r\nConnection: Close\r\nPragma: no-cache\r\nHost: %s:%d\r\nContent-Length: 304\r\nContent-Type: text/xml; charset=\"utf-8\"\r\nSOAPAction: \"urn:schemas-upnp-org:service:AVTransport:1#Play\"\r\n\r\n<?xml version=\"1.0\"?>\r\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"\r\ns:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n<s:Body>\r\n<u:Play xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\">\r\n<InstanceID>0</InstanceID>\r\n<Speed>1</Speed>\r\n</u:Play>\r\n</s:Body>\r\n</s:Envelope>";
    const CHAR msgAVTransStop[] = "POST %s HTTP/1.1\r\nCache-Control: no-cache\r\nConnection: Close\r\nPragma: no-cache\r\nHost: %s:%d\r\nContent-Length: 286\r\nContent-Type: text/xml; charset=\"utf-8\"\r\nSOAPAction: \"urn:schemas-upnp-org:service:AVTransport:1#Stop\"\r\n\r\n<?xml version=\"1.0\"?>\r\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"\r\ns:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n<s:Body>\r\n<u:Stop xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\">\r\n<InstanceID>0</InstanceID>\r\n</u:Stop>\r\n</s:Body>\r\n</s:Envelope>";

    switch (m_phase)
    {
        case PHASE_GET_PROTOCOL_INFO:
            sprintf_s(CompleteMSG, sizeof(CompleteMSG), msgConnMgr, m_url, inet_ntoa(m_DevLoc.sin_addr), ntohs(m_DevLoc.sin_port));
            break;
        case PHASE_SET_AV_TRANSPORT:
            sprintf_s(FullAVTransSet_Body, sizeof(FullAVTransSet_Body), AVTransSet_Body, GetLocalHostIP(localIP));
            sprintf_s(CompleteMSG, sizeof(CompleteMSG), msgAVTransSet_Head, m_url, inet_ntoa(m_DevLoc.sin_addr), ntohs(m_DevLoc.sin_port), strlen(FullAVTransSet_Body), FullAVTransSet_Body);
            break;
        case PHASE_SET_PLAY:
            sprintf_s(CompleteMSG, sizeof(CompleteMSG), msgAVTransPlay, m_url, inet_ntoa(m_DevLoc.sin_addr), ntohs(m_DevLoc.sin_port));
            break;
        case PHASE_SET_STOP:
            sprintf_s(CompleteMSG, sizeof(CompleteMSG), msgAVTransStop, m_url, inet_ntoa(m_DevLoc.sin_addr), ntohs(m_DevLoc.sin_port));
            break;
    }
    if(!Send(CompleteMSG, strlen(CompleteMSG)))
        return FALSE;
    return TRUE;
}

void TCPClient::OnReceive(int nErrorCode)
{
    if (nErrorCode)
    {
        CAsyncSocket::OnReceive(nErrorCode);
        return;
    }
    CHAR buffer[15000];
    int recv = Receive(buffer, sizeof(buffer));
    if (recv == SOCKET_ERROR)
    {
        CAsyncSocket::OnReceive(nErrorCode);
        return;
    }
	else
	{
        if (recv > 0)
        {
            buffer[recv] = 0;
            if (controller && (m_phase == PHASE_GET_DESCRIPTION))
                controller->OnHttpResponseReceived(buffer, TRUE, TRUE);

            if (controller && (m_phase == PHASE_SET_AV_TRANSPORT))
                controller->OnAVTransSetResp(buffer);
        }
    }
    CAsyncSocket::OnReceive(nErrorCode);
}

void TCPClient::SetController(Controller *controller)
{
    this->controller = controller;
}

LPSTR TCPClient::GetLocalHostIP(LPSTR IP)
{
    CHAR localhost[50];
    gethostname(localhost, sizeof(localhost));
    struct hostent *host = gethostbyname(localhost);
    LPSTR myIP = inet_ntoa(*(struct in_addr*)*host->h_addr_list);
    return IP = myIP;
}

void CALLBACK TCPClient::TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired)
{
    TCPClient* obj = (TCPClient*)lpParametar;
    obj->QueueTimerHandler();
}

void TCPClient::QueueTimerHandler()
{
    DeleteTimerQueueTimer(NULL, m_connectionTimer, NULL);
    if (controller)
        controller->OnConnectionTimeout();
}