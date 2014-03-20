#ifndef DEVICE_H
#define DEVICE_H

enum DevType
{
    MediaRender,
    Other,
    NotSet
};

class Device 
{
public:
    Device();
    ~Device();

    DevType GetType(){return m_type;}
    void SetType(DevType type){m_type = type;}
        
    LPSTR GetSN(){return m_SN;}
    void SetSN(LPSTR usn){strcpy_s(m_SN, _countof(m_SN), usn);}

    LPSTR GetLocation(){return m_location;}
    void SetLocation(LPSTR location){strcpy_s(m_location, _countof(m_location), location);}

    LPSTR GetMaufacturer(){return m_manufacturer;}
    void SetManufacturer(LPSTR manufacturer) {strcpy_s(m_manufacturer, _countof(m_manufacturer), manufacturer);}

    LPSTR GetModelName(){return m_modelName;}
    void SetModelName(LPSTR modelName) {strcpy_s(m_modelName, _countof(m_modelName), modelName);}

    LPSTR GetRendCtrlURL(){return m_RendCtrlURL;}
    void SetRendCtrlURL(LPSTR RendCtrlURL) {strcpy_s(m_RendCtrlURL, _countof(m_RendCtrlURL), RendCtrlURL);}

    LPSTR GetConnMgrUR(){return m_ConnMgrURL;}
    void SetConnMgrUR(LPSTR ConnMgrURL) {strcpy_s(m_ConnMgrURL, _countof(m_ConnMgrURL), ConnMgrURL);}

    LPSTR GetAVTransURL(){return m_AVTransURL;}
    void SetAVTransURL(LPSTR AVTransURL) {strcpy_s(m_AVTransURL, _countof(m_AVTransURL), AVTransURL);}

private:
    DevType m_type;
    CHAR m_manufacturer[50];
    CHAR m_modelName[50];
    CHAR m_SN[512];
    CHAR m_location[MAX_PATH];
    CHAR m_ConnMgrURL[MAX_PATH];
    CHAR m_RendCtrlURL[MAX_PATH];
    CHAR m_AVTransURL[MAX_PATH];
};

#endif