#include "Multicast.h"

MulticastSocket::MulticastSocket(){}
MulticastSocket::~MulticastSocket()
{

    closesocket(RecvSocket);
}

int MulticastSocket::Create()
{
    sockVersion = MAKEWORD(2,1);
    WSAStartup(sockVersion, &wsaData);
    //create DGRAM(UDP) socket
    RecvSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (RecvSocket == SOCKET_ERROR)
        return 0;
    //allow reuse of local port if needed
	int reuse = 1; 
    if(setsockopt(RecvSocket ,SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) == SOCKET_ERROR)
        return 0;
    //fill localSock struct
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(1900);
    localSock.sin_addr.s_addr = inet_addr("192.168.1.102");
    //bind Socket
    if(bind(RecvSocket, (struct sockaddr*)&localSock, sizeof(localSock)) == SOCKET_ERROR)
        return 0; 
    //join the multicast group 239.255.255.250 on the local interface. 
    group.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
    group.imr_interface.s_addr = inet_addr("192.168.1.102");
    if(setsockopt(RecvSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) == SOCKET_ERROR)
        return 0;
    return 1;
}

LPSTR MulticastSocket::OnRecieve(LPSTR databuf)
{
    SOCKADDR Sender;
    int SenderAddrSize = sizeof(Sender);
    int count = recvfrom(RecvSocket, databuf, 700, 0, (SOCKADDR *)&Sender, &SenderAddrSize);
    if(count != SOCKET_ERROR)
    {
        databuf[count] = 0;
        return databuf;
    }
    else
    {
        return 0;  
    }
}
