#include "Socket.h"

UnicastSocket::UnicastSocket(char ttl)
{
    sockVersion = MAKEWORD(2,1);
    WSAStartup(sockVersion, &wsaData);
    SendSocket = socket(AF_INET, SOCK_DGRAM, 0);
    ZeroMemory(&groupSock, sizeof(groupSock));
    //Disable Loopback
    char loop = '0';
    setsockopt(SendSocket, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    //Set TTL
    setsockopt(SendSocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    //Get local IP
    char HostName[255];
    gethostname(HostName, 255);
    struct hostent *host_entry;
    host_entry = gethostbyname(HostName);
    char * LocalIPA;
    LocalIPA = inet_ntoa (*(struct in_addr *)*host_entry->h_addr_list);
    LocalIP.s_addr = inet_addr(LocalIPA);
    //Set local IP for outbound multicast datagrams
    setsockopt(SendSocket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&LocalIP, sizeof(LocalIP));
    //Initialize the group sockaddr structure
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr("239.255.255.250");
    groupSock.sin_port = htons(1900);
    // Send a message to the multicast group
    TCHAR Msch[] = _T("M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nST: urn:schemas-upnp-org:device:MediaRenderer:1\r\nMAN: \"ssdp:discover\"\r\nMX:1\r\n\r\n");
    sendto(SendSocket, (char *)&Msch, sizeof(Msch), 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
    wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
}

UnicastSocket::~UnicastSocket()
{
    closesocket(SendSocket);
}

LPSTR UnicastSocket::OnReceive(LPSTR databuf)
{

    SOCKADDR Sender;
    int SenderAddrSize = sizeof(Sender);
    int count = recvfrom(SendSocket, databuf, 700, 0, (SOCKADDR *)&Sender, &SenderAddrSize);
    if((count != SOCKET_ERROR))
    {
        databuf[count] = 0;
        return databuf;
    }
    else
    {
        return NULL;
    }
}
