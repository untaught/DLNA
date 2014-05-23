#ifndef DEVLIST_H
#define DEVLIST_H
#include "Device.h"
#include "Controller.h"

class DevList
{
public:
    DevList();
    ~DevList();
    void ClearList();
    int IsDevInList(LPSTR usn_udn);
    int AddDev(LPSTR usn);
    void RemoveDevice(LPSTR usn);
    void SetController(Controller *controller);
    LPSTR static GetHTTPHeader(LPSTR response, LPSTR key, LPSTR value, int valueSize);
    LPSTR static GetHTTPBody(LPSTR response, LPSTR key, LPSTR value, int valueSize);
    Device* m_devList[250];
private:
    Controller *controller;
};

#endif