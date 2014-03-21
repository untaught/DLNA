#include "stdafx.h"
#include "Device.h"
#include "DevList.h"

DevList::DevList()
{
    ZeroMemory(m_devList, sizeof(m_devList));
}

DevList::~DevList()
{
    ClearList();
}

void DevList::ClearList()
{
    for (int i = 0; i < _countof(m_devList); i++)
    {
        if (m_devList[i]) 
        {
            delete m_devList[i];
            m_devList[i] =NULL;
        }
    }
}

LPSTR DevList::GetHTTPHeader(LPSTR response, LPSTR key, LPSTR value, int valueSize)
{
    LPSTR v = strstr(response, key);
    if (!v) return "";
    v+= strlen(key) + 1;

    int idx = 0;
    while (*v && *v!='\r')  value[idx++] = *v++;        
    value[idx] = 0;

    return value;
}

LPSTR DevList::GetHTTPBody(LPSTR response, LPSTR key, LPSTR value, int valueSize)
{
    LPSTR v = strstr(response, key);
    if (!v) return "";
    v+= strlen(key);

    int idx = 0;
    while (*v && *v!='<')  value[idx++] = *v++;
    value[idx] = 0;

    return value;
}

int DevList::AddDev(LPSTR usn)
{
    int pos = IsDevInList(usn);
    if (pos >= 0) return pos;
    pos = 0;
    while (pos < _countof(m_devList) && m_devList[pos]) pos++;
    if (m_devList[pos]) return -1;
    m_devList[pos] = new Device();
    m_devList[pos]->SetSN(usn);
    return pos;
}

int DevList::IsDevInList(LPSTR usn_udn)
{
    int fres = -1;
    for (int i = 0; i < _countof(m_devList); i++)
    {
        if (!m_devList[i]) continue;
        LPSTR cusn = m_devList[i]->GetSN();
        if (!strcmp(cusn, usn_udn) || strstr(cusn, usn_udn)) 
        {
            fres = i;
            break;
        }
    }
    return fres;
}