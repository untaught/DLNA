
// DLNA_PDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DLNA_P.h"
#include "DLNA_PDlg.h"
#include "afxwin.h"
#include "Unicast.h"
#include "Multicast.h"
#include "TCP.h"
#include "Device.h"
#include "DevList.h"
#include "Controller.h"
#include "Cap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDLNA_PDlg dialog



CDLNA_PDlg::CDLNA_PDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDLNA_PDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDLNA_PDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, CBox);
    DDX_Control(pDX, IDC_COMBO2, CBoxDev);
}

BEGIN_MESSAGE_MAP(CDLNA_PDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CDLNA_PDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CDLNA_PDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CDLNA_PDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON4, &CDLNA_PDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CDLNA_PDlg message handlers

BOOL CDLNA_PDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
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

void CDLNA_PDlg::OnBnClickedButton1()
{
    const CHAR *Msch ="M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nST: %S\r\nMAN: \"ssdp:discover\"\r\nMX:1\r\n\r\n";
    CHAR mine[1024];
    TCHAR txt[300];
	CBox.GetLBText(CBox.GetCurSel(), txt);
    sprintf_s(mine, sizeof(mine), Msch, txt);
    test.unicastSock.CreateSocket(4);
    test.unicastSock.SendMsg(mine);
}

void CDLNA_PDlg::OnBnClickedButton2()
{
    CHAR *mydev = test.devList.m_devList[CBox.GetCurSel()]->GetLocation();
    test.tcpSock.CreateSocket(mydev);
}


void CDLNA_PDlg::OnBnClickedButton3()
{
    test.multicastSock.CreateSocket();
}

void CDLNA_PDlg::OnBnClickedButton4()
{
    int i = 0;
    CHAR *t;
    while (test.devList.m_devList[i])
    {
    t = test.devList.m_devList[i]->GetSN();
    int newsize = strlen(t) + 1;
    wchar_t wcstring[200];
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wcstring, newsize, t, _TRUNCATE);
    CBoxDev.AddString(LPCTSTR(wcstring));
    i++;
    }
    CBoxDev.SetCurSel(0);
    CBoxDev.ShowWindow(SW_SHOW);
}