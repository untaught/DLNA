// DLNA_PDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DLNA_P.h"
#include "DLNA_PDlg.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDLNA_PDlg dialog

CDLNA_PDlg::CDLNA_PDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDLNA_PDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);

    // Own Declarations
    unicastSock.SetController(this);
    multicastSock.SetController(this);
    tcpSockClient.SetController(this);
    tcpSockServer.SetController(this);
    devList.SetController(this);
}

void CDLNA_PDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, CBox);
    DDX_Control(pDX, IDC_COMBO2, CBoxDev);
    DDX_Control(pDX, IDC_EDIT1, EditBox);
}

BEGIN_MESSAGE_MAP(CDLNA_PDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CDLNA_PDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CDLNA_PDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CDLNA_PDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON4, &CDLNA_PDlg::OnBnClickedButton4)
    ON_CBN_SELCHANGE(IDC_COMBO2, &CDLNA_PDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CDLNA_PDlg message handlers

BOOL CDLNA_PDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon

    BOOL OK = tcpSockServer.CreateSocket() && unicastSock.CreateSocket(4) && multicastSock.CreateSocket();
    if (!OK) 
	{
        MessageBox(_T("Could not create required sockets!"), _T("Error"), MB_OK | MB_ICONHAND);
		PostQuitMessage(-1);
		return TRUE;
	}

    EditBox.SetWindowTextW(_T("..."));
    CBox.AddString(_T("urn:schemas-upnp-org:device:MediaRenderer:1"));
    CBox.SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDLNA_PDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDLNA_PDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDLNA_PDlg::OnHttpResponseReceived(LPSTR response, BOOL xml, BOOL alive)
{
    CSingleLock lock(&m_mutex);
    lock.Lock();
    CHAR val[500];
    if (alive)
    {
        if (!xml)
        {
            devList.GetHTTPHeader(response, "USN:", val, sizeof(val));
            int idx = devList.AddDev(val);
            val[0] = 0;
            devList.GetHTTPHeader(response, "LOCATION:", val, sizeof(val));
            if (strlen(val))
                devList.m_devList[idx]->SetLocation(val);
            else
            {
                devList.GetHTTPHeader(response, "Location:", val, sizeof(val));
                if (strlen(val))
                    devList.m_devList[idx]->SetLocation(val);
                else 
                    devList.m_devList[idx]->SetLocation("");
            }
            val[0] = 0;
            devList.GetHTTPHeader(response, "ST:", val, sizeof(val));
            if (strstr(val, "MediaRenderer"))
                devList.m_devList[idx]->SetType(MediaRender);
            else
            {
                devList.GetHTTPHeader(response, "NT:", val, sizeof(val));
                if (strstr(val, "MediaRenderer"))
                    devList.m_devList[idx]->SetType(MediaRender);
                else
                devList.m_devList[idx]->SetType(Other);
            }
        }
        else
        {
            LPSTR service;
            if (devList.GetHTTPBody(response, "<UDN>", val, sizeof(val)) == "")
                return;
            int idx = devList.IsDevInList(val);
            if (idx < 0)
                return;
            val[0] = 0;
            devList.GetHTTPBody(response, "<manufacturer>", val, sizeof(val));
            devList.m_devList[idx]->SetManufacturer(val);
            val[0] = 0;
            devList.GetHTTPBody(response, "<modelName>", val, sizeof(val));
            devList.m_devList[idx]->SetModelName(val);
            val[0] = 0;
            service = strstr(response, ":RenderingControl</serviceId>");
            if (service)
                devList.GetHTTPBody(service, "<controlURL>", val, sizeof(val));
            if (strlen(val))
                devList.m_devList[idx]->SetRendCtrlURL(val);
            val[0] = 0;
            service = strstr(response, ":ConnectionManager</serviceId>");
            if (service)
                devList.GetHTTPBody(service, "<controlURL>", val, sizeof(val));
            if (strlen(val))
                devList.m_devList[idx]->SetConnMgrUR(val);
            val[0] = 0;
            service = strstr(response, ":AVTransport</serviceId>");
            if (service)
                devList.GetHTTPBody(service, "<controlURL>", val, sizeof(val));
            if (strlen(val))
            {
                devList.m_devList[idx]->SetAVTransURL(val);
                tcpSockClient.SetPhase(PHASE_SET_AV_TRANSPORT);
                tcpSockClient.CreateSocket(devList.m_devList[CBoxDev.GetCurSel()]->GetLocation(), devList.m_devList[CBoxDev.GetCurSel()]->GetAVTransURL());
            }
        }
    }
    else
    {
        devList.GetHTTPHeader(response, "USN:", val, sizeof(val));
        devList.RemoveDevice(val);
    }
    lock.Unlock();
}
void CDLNA_PDlg::OnConnectionTimeout()
{
    MessageBox(_T("Device is unreachable!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
    GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
    GetDlgItem(IDC_COMBO2)->EnableWindow(true);
}

void CDLNA_PDlg::OnFileOpenError()
{
    tcpSockClient.SetPhase(PHASE_SET_STOP);
    tcpSockClient.SendMsgSOAP();
    MessageBox(_T("File not found!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
}

void CDLNA_PDlg::OnAVTransSetResp(LPSTR response)
{
    if (strstr(response, "SetAVTransportURIResponse"))
    {
        if (!strstr(response, "error"))
        {
            tcpSockClient.SetPhase(PHASE_SET_PLAY);
            tcpSockClient.CreateSocket(devList.m_devList[CBoxDev.GetCurSel()]->GetLocation(), devList.m_devList[CBoxDev.GetCurSel()]->GetAVTransURL());
        }
    }
    else if (strstr(response, "<errorCode>704</errorCode>"))
    {
        GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
        GetDlgItem(IDC_COMBO2)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON3)->EnableWindow(true);
        MessageBox(_T("File format not supported!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
    }
    else if (strstr(response, "<errorCode>"))
    {
        GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
        GetDlgItem(IDC_COMBO2)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON3)->EnableWindow(true);
        MessageBox(_T("Communication error!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
    }
}
void CDLNA_PDlg::OnSearchStarted()
{
    CBoxDev.ShowWindow(true);
    CBoxDev.EnableWindow(false);
    CBoxDev.ResetContent();
    CBoxDev.AddString(_T("Searching for devices..."));
    CBoxDev.SetCurSel(0);
    GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
    GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
    EditBox.SetWindowTextW(_T("..."));
}

void CDLNA_PDlg::OnSearchComplete()
{
    CBoxDev.ResetContent();
    int i = 0;
    while (devList.m_devList[i])
    {
        CBoxDev.AddString(CA2W(devList.m_devList[i]->GetSN()));
        i++;
    }
    CBoxDev.SetCurSel(0);
    WCHAR txt[300];
    txt[0] = 0;
    CBoxDev.GetLBText(CBoxDev.GetCurSel(), txt);
    if (wcslen(txt))
    {
        CBoxDev.EnableWindow(true);
        GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
    }
    else
    {
        CBoxDev.AddString(_T("No devices found!"));
        CBoxDev.SetCurSel(0);
        GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
    }
}

void CDLNA_PDlg::OnBadDeviceLocation()
{
    MessageBox(_T("Device does not support streaming content!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
    GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON3)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
    CBoxDev.EnableWindow(true);
}

void CDLNA_PDlg::OnNewDeviceFound()
{
    int selected = CBoxDev.GetCurSel();
    WCHAR txt[300];
    CBoxDev.GetLBText(selected, txt);
    if (!wcscmp(txt, _T("No devices found!")))
        selected = -1;
    CBoxDev.ResetContent();
    int i = 0;
    while (devList.m_devList[i])
    {
        CBoxDev.AddString(CA2W(devList.m_devList[i]->GetSN()));
        i++;
    }
    CBoxDev.SetCurSel(selected);
    CBoxDev.ShowWindow(true);
    if (GetDlgItem(IDC_BUTTON4)->IsWindowEnabled())
        CBoxDev.EnableWindow(false);
    else
        CBoxDev.EnableWindow(true);
}

void CDLNA_PDlg::OnDeviceRemoved()
{
    OnNewDeviceFound();
}

void CDLNA_PDlg::OnNoNotifyForTooLong(LPSTR sn)
{
    devList.RemoveDevice(sn);
}


void CDLNA_PDlg::OnBnClickedButton1()
{ 
    WCHAR txt[300];
	CBox.GetLBText(CBox.GetCurSel(), txt);
    devList.ClearList();
    unicastSock.SendMsg(CW2A(txt));
}

void CDLNA_PDlg::OnBnClickedButton2()
{
    CFileDialog fOpenDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Audio files (*.wav, *.mp3)|*.wav;*.mp3|Image files (*.bmp, *.jpg, *.jpeg, *.png, *.gif, *.tif, *.tiff)|*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff|Video files (*.avi, *.mp4, *.mkv)|*.avi;*.mp4;*.mkv||"), this, 0, FALSE);
    
	if(fOpenDlg.DoModal() == IDOK)
	{
        CString file = fOpenDlg.GetPathName();
        tcpSockServer.SetFilePath(CW2A(file));
	    EditBox.SetWindowTextW(file);
        GetDlgItem(IDC_BUTTON3)->EnableWindow(true);
        GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
    }
}

void CDLNA_PDlg::OnBnClickedButton3()
{
    tcpSockClient.SetPhase(PHASE_GET_DESCRIPTION);
    if (tcpSockClient.CreateSocket(devList.m_devList[CBoxDev.GetCurSel()]->GetLocation()))
    {
        GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON4)->EnableWindow(true);
        GetDlgItem(IDC_COMBO2)->EnableWindow(false);
    }
}

void CDLNA_PDlg::OnBnClickedButton4()
{
    tcpSockClient.SetPhase(PHASE_SET_STOP);
    tcpSockClient.CreateSocket(devList.m_devList[CBoxDev.GetCurSel()]->GetLocation(), devList.m_devList[CBoxDev.GetCurSel()]->GetAVTransURL());
    GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON3)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
    GetDlgItem(IDC_COMBO2)->EnableWindow(true);
}


void CDLNA_PDlg::OnCbnSelchangeCombo2()
{
    int selected = CBoxDev.GetCurSel();
    WCHAR txt[300];
    CBoxDev.GetLBText(selected, txt);
    if (!wcscmp(txt, _T("No devices found!")))
        GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
    else
        GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
}
