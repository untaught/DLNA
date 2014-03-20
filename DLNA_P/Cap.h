#ifndef CAP_H
#define CAP_H

class Cap: public Controller
{
public:
    Cap();
    ~Cap();
    virtual void OnHttpResponseReceived(LPSTR response, BOOL xml);

public:
    TCP tcpSock;
    Unicast unicastSock;
    Multicast multicastSock;
    DevList devList;
};

#endif