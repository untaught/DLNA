#ifndef DEVLIST_H
#define DEVLIST_H

class DevList
{
public:
    DevList();
    ~DevList();
    void ClearList();
    int IsDevInList(LPSTR usn_udn);
    int AddDev(LPSTR usn);

    LPSTR static GetHTTPHeader(LPSTR response, LPSTR key, LPSTR value, int valueSize);
    LPSTR static GetHTTPBody(LPSTR response, LPSTR key, LPSTR value, int valueSize);

    Device* m_devList[25];
};

#endif