#include "stdafx.h"
#include "Device.h"

Device::Device(Controller *controller)
{
    this->controller = controller;
    m_type = NotSet;
    ZeroMemory(m_manufacturer,sizeof(m_manufacturer));
    ZeroMemory(m_modelName,sizeof(m_modelName));
    ZeroMemory(m_SN,sizeof(m_SN));
    ZeroMemory(m_location,sizeof(m_location));
    ZeroMemory(m_ConnMgrURL,sizeof(m_ConnMgrURL));
    ZeroMemory(m_RendCtrlURL,sizeof(m_RendCtrlURL));
    ZeroMemory(m_AVTransURL,sizeof(m_AVTransURL));
    CreateTimerQueueTimer(&m_keepAliveTimer, NULL, Device::TimerProc, this, 30000, 0, 0);
}

Device::~Device()
{
    DeleteTimerQueueTimer(NULL, m_keepAliveTimer, NULL);
}

void CALLBACK Device::TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired)
{
    Device* obj = (Device*)lpParametar;
    obj->QueueTimerHandler();
}

void Device::QueueTimerHandler()
{
    if (controller)
        controller->OnNoNotifyForTooLong(m_SN);
}

void Device::RefreshTimer()
{
    ChangeTimerQueueTimer(NULL, m_keepAliveTimer, 30000, INFINITE);
}