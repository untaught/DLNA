#include "Socket.h"
//TO DO: 
    //Set LocalIP.s_addr to LocalIPA
UnicastSocket::UnicastSocket(){}

UnicastSocket::~UnicastSocket()
{
    closesocket(SendSocket);
}

int UnicastSocket::Create(char ttl)
{
    sockVersion = MAKEWORD(2,1);
    WSAStartup(sockVersion, &wsaData);
    //Create DGRAM(UDP) Socket
    SendSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (SendSocket == INVALID_SOCKET)
        return 0;
    ZeroMemory(&groupSock, sizeof(groupSock));
    //Disable Loopback
    char loop = 0;
    if(setsockopt(SendSocket, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) == SOCKET_ERROR)
    {
        this->~UnicastSocket();
        return 0;
    }
    //Set timeout if not data is received
    int time = 2000;
    if(setsockopt(SendSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&time,sizeof(time)) == SOCKET_ERROR)
    {
        this->~UnicastSocket();
        return 0;
    }
    //Set TTL
    if(setsockopt(SendSocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) == SOCKET_ERROR)
    {
        this->~UnicastSocket();
        return 0;
    }
    //Get local IP
    char HostName[255];
    gethostname(HostName, 255);
    struct hostent *host_entry;
    host_entry = gethostbyname(HostName);
    char * LocalIPA;
    LocalIPA = inet_ntoa (*(struct in_addr *)*host_entry->h_addr_list);
    LocalIP.s_addr = inet_addr("192.168.1.102");
    //Set local IP for outbound multicast datagrams
    if(setsockopt(SendSocket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&LocalIP, sizeof(LocalIP)) == SOCKET_ERROR)
    {
        this->~UnicastSocket();
        return 0;
    }
    return 1;
}

int UnicastSocket::Send()
{
    //Initialize the group sockaddr structure
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr("239.255.255.250");
    groupSock.sin_port = htons(1900);
    // Send a message to the multicast group
    TCHAR Msch[] = _T("M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nST: urn:schemas-upnp-org:device:MediaRenderer:1\r\nMAN: \"ssdp:discover\"\r\nMX:1\r\n\r\n");
    if (sendto(SendSocket, Msch, sizeof(Msch), 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) == SOCKET_ERROR)
    {
        return 0;
        this->~UnicastSocket();
    }
    return 1;
}

LPSTR UnicastSocket::OnReceive(LPSTR databuf)
{

    SOCKADDR Sender;
    int SenderAddrSize = sizeof(Sender);
    int count = recvfrom(SendSocket, databuf, 700, 0, (SOCKADDR *)&Sender, &SenderAddrSize);
    if (WSAGetLastError() == WSAETIMEDOUT)
    {
        databuf[0]=0;
        WSACleanup();
        return databuf;
    }
    //SOCKET_ERROR = -1; Closed Connection = 0;
    if((count > 0))
    {
        databuf[count] = 0;
        return databuf;
    }
    else
    {
        return NULL;
    }
}
