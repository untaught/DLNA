#include "stdafx.h"

#include "Unicast.h"
#include "Multicast.h"
#include "TCP.h"
#include "Device.h"
#include "DevList.h"
#include "Controller.h"
#include "Cap.h"

Cap::Cap()
{
    unicastSock.SetController(this);
    multicastSock.SetController(this);
    tcpSock.SetController(this);
};
Cap::~Cap()
{
};
void Cap::OnHttpResponseReceived(LPSTR response, BOOL xml)
{
    CHAR val[500];
    if(!xml)
    {
        devList.GetHTTPHeader(response, "USN:", val, strlen(val));
        int idx = devList.AddDev(val);

        devList.GetHTTPHeader(response, "LOCATION:", val, strlen(val));
        devList.m_devList[idx]->SetLocation(val);

        devList.GetHTTPHeader(response, "ST:", val, strlen(val));
        if (strstr(val, "MediaRenderer"))
            devList.m_devList[idx]->SetType(MediaRender);
        else
            devList.m_devList[idx]->SetType(Other);
    }
    else
    {
        LPSTR service;
        if (devList.GetHTTPBody(response, "<UDN>", val, strlen(val)) == "") return;
        int idx = devList.IsDevInList(val);
        if (idx < 0) return;
        
        devList.GetHTTPBody(response, "<manufacturer>", val, strlen(val));
        devList.m_devList[idx]->SetManufacturer(val);

        devList.GetHTTPBody(response, "<modelName>", val, strlen(val));
        devList.m_devList[idx]->SetModelName(val);

        service = strstr(response, ":RenderingControl</serviceId>");
        devList.GetHTTPBody(service, "<controlURL>", val, strlen(val));
        devList.m_devList[idx]->SetRendCtrlURL(val);

        service = strstr(response, ":ConnectionManager</serviceId>");
        devList.GetHTTPBody(service, "<controlURL>", val, strlen(val));
        devList.m_devList[idx]->SetConnMgrUR(val);

        service = strstr(response, ":AVTransport</serviceId>");
        devList.GetHTTPBody(service, "<controlURL>", val, strlen(val));
        devList.m_devList[idx]->SetAVTransURL(val);
    }
}