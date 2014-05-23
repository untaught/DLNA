#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller
{
public:
	virtual void OnHttpResponseReceived(LPSTR response, BOOL xml, BOOL alive) = 0;
    virtual void OnConnectionTimeout() = 0;
    virtual void OnAVTransSetResp(LPSTR response) = 0;
    virtual void OnFileOpenError() = 0;
    virtual void OnSearchStarted() = 0;
    virtual void OnSearchComplete() = 0;
    virtual void OnBadDeviceLocation() = 0;
    virtual void OnNewDeviceFound() = 0;
    virtual void OnDeviceRemoved() = 0;
    virtual void OnNoNotifyForTooLong(LPSTR sn) = 0;
};

#endif