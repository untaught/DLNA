#include "stdafx.h"
#include "Device.h"

Device::Device()
{
    m_type = NotSet;
    ZeroMemory(m_manufacturer,sizeof(m_manufacturer));
    ZeroMemory(m_modelName,sizeof(m_modelName));
    ZeroMemory(m_SN,sizeof(m_SN));
    ZeroMemory(m_location,sizeof(m_location));
    ZeroMemory(m_ConnMgrURL,sizeof(m_ConnMgrURL));
    ZeroMemory(m_RendCtrlURL,sizeof(m_RendCtrlURL));
    ZeroMemory(m_AVTransURL,sizeof(m_AVTransURL));
}

Device::~Device()
{
}
