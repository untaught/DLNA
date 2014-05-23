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

void DevList::SetController(Controller *controller)
{
    this->controller = controller;
}

void DevList::ClearList()
{
    for (int i = 0; i < _countof(m_devList); i++)
    {
        if (m_devList[i]) 
        {
            delete m_devList[i];
            m_devList[i] = NULL;
        }
    }
}

LPSTR DevList::GetHTTPHeader(LPSTR response, LPSTR key, LPSTR value, int valueSize)
{
    if (!value) return "";
    value[0] = 0;
    if (!key || !strlen(key)) return value;
    if (!response) return value;
    CHAR lkey[512];
    strcpy_s(lkey, sizeof(lkey), key);
    if (lkey[strlen(lkey) - 1] != ':') strcat_s(lkey, sizeof(lkey), ":");
    LPSTR v = strstr(response, lkey);
    if (!v) return "";
    v += strlen(lkey);

    int idx = 0;
    while (*v && /**v != '\r' && *v != '\n' &&*/ isspace(*v))			v++; // strip leading whitespace
    while (*v && *v != '\r' && *v != '\n' && ((idx + 1) < valueSize))		value[idx++] = *v++;
    value[idx] = 0;
    while (strlen(value) && isspace(value[strlen(value) - 1]))	value[strlen(value) - 1] = 0;	// strip trailing whitespace

    return value;

    /*LPSTR v = strstr(response, key);
    if (!v) return "";
    v += strlen(key) + 1;

    int idx = 0;
    while (*v && *v!='\r')  value[idx++] = *v++;        
    value[idx] = 0;

    return value;*/
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
    m_devList[pos] = new Device(controller);
    m_devList[pos]->SetSN(usn);
    controller->OnNewDeviceFound();
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
            m_devList[i]->RefreshTimer();
            break;
        }
    }
    return fres;
}

void DevList::RemoveDevice(LPSTR usn)
{
    int pos = IsDevInList(usn);
    if (pos < 0) return;
    delete m_devList[pos];
    m_devList[pos] = NULL;

    //Reorder Array
    for (int i = pos; i < _countof(m_devList) - 1; i++)
        m_devList[i] = m_devList[i + 1];
    if (m_devList[_countof(m_devList) - 1])
    {
        delete m_devList[_countof(m_devList) - 1];
        m_devList[_countof(m_devList) - 1] = NULL;
    }
    controller->OnDeviceRemoved();
}